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

#include "bhv_set_play_kick_in.h"

#include "bhv_set_play.h"
#include "bhv_prepare_set_play_kick.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_advance_ball.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_kick_one_step.h>
#include <rcsc/action/body_kick_collide_with_ball.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/neck_scan_field.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

/*-------------------------------------------------------------------*/
/*!
  execute action
*/
bool
Bhv_SetPlayKickIn::execute( rcsc::PlayerAgent * agent )
{
    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        "%s:%d: Bhv_SetPlayKickIn"
                        ,__FILE__, __LINE__ );

    if ( isKicker( agent ) )
    {
        doKick( agent );
    }
    else
    {
        doMove( agent );
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_SetPlayKickIn::isKicker( const rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();

    if ( wm.teammatesFromBall().empty() )
    {
        return true;
    }

    long max_wait1 = 30;
    long max_wait2 = 50;

    const rcsc::PlayerObject * nearest_mate
        = static_cast< rcsc::PlayerObject * >( 0 );
    nearest_mate = wm.teammatesFromBall().front();

    if ( wm.setplayCount() < max_wait1
         || ( wm.setplayCount() < max_wait2
              && wm.self().pos().dist( M_home_pos ) > 20.0 )
         || ( nearest_mate
              && nearest_mate->distFromBall() < wm.ball().distFromSelf() * 0.9 )
         )
    {
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
Bhv_SetPlayKickIn::doKick( rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();

    rcsc::AngleDeg ball_place_angle = ( wm.ball().pos().y > 0.0
                                        ? -90.0
                                        : 90.0 );

    if ( Bhv_PrepareSetPlayKick( ball_place_angle, 10 ).execute( agent ) )
    {
        // go to kick point
        return;
    }


    rcsc::Vector2D target_point;
    double ball_speed;

    if  ( rcsc::Body_Pass::get_best_pass( wm,
                                          & target_point,
                                          & ball_speed,
                                          NULL )
          && target_point.x > -25.0
          && target_point.x < 48.0 )
    {
        // enforce one step kick
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: pass to (%f, %f)"
                            ,__FILE__, __LINE__,
                            target_point.x, target_point.y );
        rcsc::Body_KickOneStep( target_point,
                                ball_speed
                                ).execute( agent );
        agent->setNeckAction( new rcsc::Neck_ScanField() );
    }
    else if ( wm.self().pos().x < 20.0 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: advance"
                            ,__FILE__, __LINE__);
        rcsc::Body_AdvanceBall().execute( agent );
        agent->setNeckAction( new rcsc::Neck_ScanField() );
    }
    else
    {
        rcsc::Vector2D target_point( rcsc::ServerParam::i().pitchHalfLength() - 2.0,
                                     ( rcsc::ServerParam::i().pitchHalfWidth() - 5.0 )
                                     * ( 1.0 - ( wm.self().pos().x
                                                 / rcsc::ServerParam::i().pitchHalfLength() ) ) );
        if ( wm.self().pos().y < 0.0 )
        {
            target_point.y *= -1.0;
        }
        // enforce one step kick
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: advance 2 to (%f, %f)"
                            ,__FILE__, __LINE__,
                            target_point.x, target_point.y );
        rcsc::Body_KickOneStep( target_point,
                                rcsc::ServerParam::i().ballSpeedMax()
                                ).execute( agent );
        agent->setNeckAction( new rcsc::Neck_ScanField() );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
Bhv_SetPlayKickIn::doMove( rcsc::PlayerAgent * agent )
{
    rcsc::Vector2D target_point = M_home_pos;

    double dash_power
        = Bhv_SetPlay::get_set_play_dash_power( agent );
    double dist_thr = agent->world().ball().distFromSelf() * 0.07;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    agent->debugClient().addMessage( "KickInMove" );
    agent->debugClient().setTarget( target_point );

    if ( ! rcsc::Body_GoToPoint( target_point,
                                 dist_thr,
                                 dash_power
                                 ).execute( agent ) )
    {
        rcsc::Body_TurnToBall().execute( agent );
    }
    agent->setNeckAction( new rcsc::Neck_ScanField() );
}
