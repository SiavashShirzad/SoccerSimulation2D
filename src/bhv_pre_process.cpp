// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_pre_process.h"

#include "strategy.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/bhv_before_kick_off.h>
#include <rcsc/action/bhv_emergency.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/body_kick_one_step.h>
#include <rcsc/action/bhv_shoot.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/view_synch.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/intercept_table.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/audio_memory.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

/*-------------------------------------------------------------------*/
/*!
  execute action
*/
bool
Bhv_PreProcess::execute( rcsc::PlayerAgent * agent )
{
    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        "%s:%d: Bhv_PreProcess"
                        ,__FILE__, __LINE__ );

    //////////////////////////////////////////////////////////////
    // freezed by tackle effect
    if ( agent->world().self().isFreezed() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: tackle wait. expires= %d"
                            ,__FILE__, __LINE__,
                            agent->world().self().tackleExpires() );
        // face neck to ball
        agent->setViewAction( new rcsc::View_Synch() );
        agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        return true;
    }

    //////////////////////////////////////////////////////////////
    // BeforeKickOff or AfterGoal. should jump to the initial position
    if ( agent->world().gameMode().type() == rcsc::GameMode::BeforeKickOff
         || agent->world().gameMode().type() == rcsc::GameMode::AfterGoal_ )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: before_kick_off"
                            ,__FILE__, __LINE__ );
        rcsc::Vector2D move_point =  Strategy::i().getPosition( agent->world().self().unum() );
        //agent->setViewAction( new rcsc::View_Synch() );
        agent->setViewAction( new rcsc::View_Wide() );
        rcsc::Bhv_BeforeKickOff( move_point ).execute( agent );
        return true;
    }

    //////////////////////////////////////////////////////////////
    // my pos is unknown
    if ( ! agent->world().self().posValid() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: invalid my pos"
                            ,__FILE__, __LINE__ );
        // included change view
        rcsc::Bhv_Emergency().execute( agent );
        return true;
    }
    //////////////////////////////////////////////////////////////
    // ball search
    // included change view
    int count_thr = 5;
    if ( agent->world().self().goalie() ) count_thr = 10;
    if ( agent->world().ball().posCount() > count_thr
         || agent->world().ball().rposCount() > count_thr + 3 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: search ball"
                            ,__FILE__, __LINE__ );
        //agent->setViewAction( new rcsc::View_Synch() );
        agent->setViewAction( new rcsc::View_Wide() );
        rcsc::Bhv_NeckBodyToBall().execute( agent );
        return true;
    }

    //////////////////////////////////////////////////////////////
    // default change view

    agent->setViewAction( new rcsc::View_Synch() );


    //////////////////////////////////////////////////////////////
    // check queued action
    if ( agent->doIntention() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: do queued intention"
                            ,__FILE__, __LINE__ );
        return true;
    }

    //////////////////////////////////////////////////////////////
    // check shoot chance
    if ( agent->world().gameMode().type() != rcsc::GameMode::IndFreeKick_
         && agent->world().self().isKickable()
         && rcsc::Bhv_Shoot2008().execute( agent ) )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": shooted" );

        // reset intention
        agent->setIntention( static_cast< rcsc::SoccerIntention * >( 0 ) );
        return true;
    }

    //////////////////////////////////////////////////////////////
    // check simultaneous kick
    if ( agent->world().gameMode().type() == rcsc::GameMode::PlayOn
         && ! agent->world().self().goalie()
         && agent->world().self().isKickable()
         && agent->world().existKickableOpponent() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: simultaneous kick"
                            ,__FILE__, __LINE__ );
        agent->debugClient().addMessage( "SimultaneousKick" );
        rcsc::Vector2D goal_pos( rcsc::ServerParam::i().pitchHalfLength(), 0.0 );

        if ( agent->world().self().pos().x > 36.0
             && agent->world().self().pos().absY() > 10.0 )
        {
            goal_pos.x = 45.0;
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                "%s:%d: simultaneous kick cross type"
                                ,__FILE__, __LINE__ );
        }
        rcsc::Body_KickOneStep( goal_pos,
                                rcsc::ServerParam::i().ballSpeedMax()
                                ).execute( agent );
        agent->setNeckAction( new rcsc::Neck_ScanField() );
        return true;
    }

    //////////////////////////////////////////////////////////////
    // check communication intention
    if ( agent->world().audioMemory().passTime() == agent->world().time()
         && ! agent->world().audioMemory().pass().empty()
         && ( agent->world().audioMemory().pass().front().receiver_
              == agent->world().self().unum() )
         )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: communication receive"
                            ,__FILE__, __LINE__ );
        doReceiveMove( agent );

        return true;
    }

    return false;
}


/*-------------------------------------------------------------------*/
/*!

*/
void
Bhv_PreProcess::doReceiveMove( rcsc::PlayerAgent * agent )
{
    agent->debugClient().addMessage( "IntentionRecv" );

    const rcsc::WorldModel & wm = agent->world();
    int self_min = wm.interceptTable()->selfReachCycle();

    if ( ( ! wm.existKickableTeammate()
           && self_min < 6 )
         || wm.audioMemory().pass().empty() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: PreProcess. Receiver. intercept cycle=%d"
                            ,__FILE__, __LINE__,
                            self_min );
        agent->debugClient().addMessage( "Intercept_1" );
        rcsc::Body_Intercept().execute( agent );
        agent->setNeckAction( new rcsc::Neck_TurnToBall() );
        return;
    }

    rcsc::Vector2D receive_pos = wm.audioMemory().pass().front().receive_pos_;

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        "%s:%d: PreProcess. Receiver. intercept cycle=%d. go to receive point"
                        ,__FILE__, __LINE__,
                        self_min );
    agent->debugClient().setTarget( receive_pos );
    agent->debugClient().addMessage( "GoTo" );
    rcsc::Body_GoToPoint( receive_pos,
                          0.5,
                          rcsc::ServerParam::i().maxDashPower()
                          ).execute( agent );
    agent->setNeckAction( new rcsc::Neck_TurnToBall() );
}
