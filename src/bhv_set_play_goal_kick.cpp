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

#include "bhv_set_play_goal_kick.h"

#include "bhv_set_play.h"
#include "bhv_prepare_set_play_kick.h"
#include "bhv_go_to_static_ball.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_clear_ball.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/neck_scan_field.h>

#include <rcsc/player/player_agent.h>

#include <rcsc/common/logger.h>

/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_SetPlayGoalKick::execute( rcsc::PlayerAgent * agent )
{
    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        "%s:%d: Bhv_SetPlayGoalKick"
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
Bhv_SetPlayGoalKick::isKicker( const rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();

    if ( agent->world().setplayCount() < 30 )
    {
        return false;
    }

    if ( wm.teammatesFromBall().empty() )
    {
        return true;
    }

    const rcsc::PlayerPtrCont::const_iterator end
        = agent->world().teammatesFromBall().end();
    for ( rcsc::PlayerPtrCont::const_iterator it
              = agent->world().teammatesFromBall().begin();
          it != end;
          ++it )
    {
        if ( ! (*it)->goalie()
             && (*it)->distFromBall() < agent->world().ball().distFromSelf() )
        {
            // exist other kicker
            return false;
        }
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
Bhv_SetPlayGoalKick::doKick( rcsc::PlayerAgent * agent )
{
    static int S_scan_count = -5;

    // go to point to kick the ball
    if ( Bhv_GoToStaticBall( 0.0 ).execute( agent ) )
    {
        return;
    }

    // already kick point
    if ( ( agent->world().ball().angleFromSelf()
           - agent->world().self().body() ).abs() > 3.0 )
    {
        rcsc::Body_TurnToBall().execute( agent );
        agent->setNeckAction( new rcsc::Neck_ScanField() );
        return;
    }

    // wait1
    if ( S_scan_count < 0 )
    {
        S_scan_count++;
        agent->setNeckAction( new rcsc::Neck_ScanField() );
        return;
    }

    // wait2
    if ( S_scan_count < 30
         && agent->world().teammatesFromSelf().empty() )
    {
        S_scan_count++;
        agent->setNeckAction( new rcsc::Neck_ScanField() );
        return;
    }

    S_scan_count = -5;

    // kick to teammate
    rcsc::Vector2D target_point;
    if  ( rcsc::Body_Pass::get_best_pass( agent->world(),
                                          & target_point,
                                          NULL,
                                          NULL )
          && target_point.dist( rcsc::Vector2D(-50.0, 0.0) ) > 20.0
          )
    {
        double opp_dist = 100.0;
        const rcsc::PlayerObject * opp
            = agent->world().getOpponentNearestTo( target_point,
                                                   10,
                                                   & opp_dist );
        if ( ! opp
             || opp_dist > 5.0 )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                "%s:%d: pass to (%f, %f)"
                                ,__FILE__, __LINE__,
                                target_point.x, target_point.y );
            rcsc::Body_Pass().execute( agent );
            agent->setNeckAction( new rcsc::Neck_ScanField() );
            return;
        }
    }

    // clear
    rcsc::Body_ClearBall().execute( agent );
    agent->setNeckAction( new rcsc::Neck_ScanField() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
Bhv_SetPlayGoalKick::doMove( rcsc::PlayerAgent * agent )
{
    double dash_power
        = Bhv_SetPlay::get_set_play_dash_power( agent );
    double dist_thr = agent->world().ball().distFromSelf() * 0.07;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    rcsc::Vector2D move_point = M_home_pos;
    move_point.y += agent->world().ball().pos().y * 0.5;

    if ( ! rcsc::Body_GoToPoint( move_point,
                                 dist_thr,
                                 dash_power
                                 ).execute( agent ) )
    {
        // already there
        rcsc::Body_TurnToBall().execute( agent );
    }
    agent->setNeckAction( new rcsc::Neck_ScanField() );
}
