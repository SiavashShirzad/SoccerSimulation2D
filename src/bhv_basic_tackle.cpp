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

#include "bhv_basic_tackle.h"

#include <rcsc/action/body_tackle_to_point.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/ray_2d.h>

//#define DEBUG

/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_BasicTackle::execute( rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();

    if ( wm.self().tackleProbability() < M_min_probability )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": failed. low tackle_prob=%.2f < %.2f",
                            wm.self().tackleProbability(),
                            M_min_probability );
        return false;
    }

    const int self_min = wm.interceptTable()->selfReachCycle();
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    bool ball_shall_be_in_our_goal = false;

    //
    // check where the ball shall be gone without tackle
    //
    const double goal_half_width = rcsc::ServerParam::i().goalHalfWidth();

    const rcsc::Vector2D goal_center = rcsc::ServerParam::i().ourTeamGoalPos();
    const rcsc::Vector2D goal_left_post( goal_center.x, +goal_half_width );
    const rcsc::Vector2D goal_right_post( goal_center.x, -goal_half_width );
    bool is_shoot_ball = ( ( (goal_left_post - wm.ball().pos() ).th()
                             - wm.ball().vel().th() ).degree() < 0
                           && ( ( goal_right_post - wm.ball().pos() ).th()
                                - wm.ball().vel().th() ).degree() > 0 );

    const int self_reach_cycle = wm.interceptTable()->selfReachCycle();

    if ( is_shoot_ball
         && wm.ball().inertiaPoint( self_reach_cycle ).x
            <= rcsc::ServerParam::i().ourTeamGoalLineX() )
    {
        ball_shall_be_in_our_goal = true;
    }

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": ball_shall_be_in_our_goal = %s",
                        ( ball_shall_be_in_our_goal ? "true" : "false" ) );

    if ( wm.existKickableOpponent()
         || ball_shall_be_in_our_goal
         || ( opp_min < self_min - 6
              && opp_min < mate_min - 6 ) )
    {

    }
    else
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": Bhv_BasicTackle. not necessary" );
        return false;
    }

    //
    // v11 or older
    //

    if ( agent->config().version() < 12.0 )
    {
        return executeOld( agent );

    }

    //
    // v12+
    //

    return executeV12( agent );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_BasicTackle::executeOld( rcsc::PlayerAgent * agent )
{
    const rcsc::WorldModel & wm = agent->world();
    const rcsc::ServerParam & param = rcsc::ServerParam::i();

    double tackle_power = param.maxTacklePower();
    if ( wm.self().body().abs() < M_body_thr )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": Bhv_BasicTackle. (old) body dir" );

        agent->debugClient().addMessage( "Tackle+" );
        agent->doTackle( tackle_power );
        agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        return true;
    }

    tackle_power = - param.maxBackTacklePower();
    if ( tackle_power < 0.0
         && wm.self().body().abs() > 180.0 - M_body_thr )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": Bhv_BasicTackle. (old) body reverse dir" );

        agent->debugClient().addMessage( "Tackle-" );
        agent->doTackle( tackle_power );
        agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        return true;
    }

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": Bhv_BasicTackle. (old) failed." );
    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_BasicTackle::executeV12( rcsc::PlayerAgent * agent )
{
    static rcsc::GameTime s_last_execute_time( 0, 0 );
    static bool s_result = false;
    static rcsc::AngleDeg s_best_angle = 0.0;

    const rcsc::WorldModel & wm = agent->world();

    if ( s_last_execute_time == wm.time() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": called several times" );
        if ( s_result )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": executeV12() angle=%.0f", s_best_angle.degree() );
            agent->debugClient().addMessage( "BasicTackle%.0f", s_best_angle.degree() );
            agent->doTackle( ( s_best_angle - wm.self().body() ).degree() );
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        return s_result;
    }
    s_last_execute_time = wm.time();
    s_result = false;

    const rcsc::ServerParam & param = rcsc::ServerParam::i();
    const rcsc::PlayerPtrCont::const_iterator o_end = wm.opponentsFromBall().end();

    const rcsc::Vector2D opp_goal( + param.pitchHalfLength(), 0.0 );
    const rcsc::Vector2D our_goal( - param.pitchHalfLength(), 0.0 );
    const rcsc::Vector2D virtual_accel = ( wm.existKickableOpponent()
                                           ? ( our_goal - wm.ball().pos() ).setLengthVector( 0.5 )
                                           : rcsc::Vector2D( 0.0, 0.0 ) );
    const bool shoot_chance = ( wm.ball().pos().dist( opp_goal ) < 20.0 );


    const rcsc::AngleDeg ball_rel_angle
        = wm.ball().angleFromSelf() - wm.self().body();
    const double tackle_rate
        = param.tacklePowerRate()
        * ( 1.0 - 0.5 * ball_rel_angle.abs() / 180.0 );

    rcsc::AngleDeg best_angle = 0.0;
    double max_speed = -1.0;

    for ( double a = -180.0; a < 180.0; a += 10.0 )
    {
        rcsc::AngleDeg rel_angle = a - wm.self().body().degree();

        double eff_power = param.maxBackTacklePower()
            + ( ( param.maxTacklePower() - param.maxBackTacklePower() )
                * ( 1.0 - rel_angle.abs() / 180.0 ) );
        eff_power *= tackle_rate;

        rcsc::Vector2D vel = wm.ball().vel()
            + rcsc::Vector2D::polar2vector( eff_power, rcsc::AngleDeg( a ) );
        vel += virtual_accel;

        if ( vel.th().abs() > 90.0 )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": executeV12() angle=%.1f vel=(%.1f %.1f)"
                                " not a forward tackle.",
                                a, vel.x, vel.y );
            continue;
        }

        const rcsc::Vector2D ball_next = wm.ball().pos() + vel;

        bool maybe_opponent_get_ball = false;
        for ( rcsc::PlayerPtrCont::const_iterator o = wm.opponentsFromBall().begin();
              o != o_end;
              ++o )
        {
            if ( (*o)->posCount() > 10 ) continue;
            if ( (*o)->isGhost() ) continue;
            if ( (*o)->isTackling() ) continue;
            if ( (*o)->distFromBall() > 6.0 ) break;;

            rcsc::Vector2D opp_pos = (*o)->pos() + (*o)->vel();
            if ( opp_pos.dist( ball_next ) < 1.0 )
            {
                maybe_opponent_get_ball = true;
                break;
            }
        }

        if ( maybe_opponent_get_ball )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": executeV12() angle=%.1f vel=(%.1f %.1f)"
                                " maybe opponent get ball.",
                                a, vel.x, vel.y );
            continue;
        }


        double speed = vel.r();
        if ( shoot_chance )
        {
            const rcsc::Ray2D ball_ray( wm.ball().pos(), vel.th() );
            const rcsc::Line2D goal_line( rcsc::Vector2D( param.pitchHalfLength(), 10.0 ),
                                          rcsc::Vector2D( param.pitchHalfLength(), -10.0 ) );
            const rcsc::Vector2D intersect =  ball_ray.intersection( goal_line );
            if ( intersect.valid()
                 && intersect.absY() < param.goalHalfWidth() - 3.0 )
            {
                speed += 10.0;
                rcsc::dlog.addText( rcsc::Logger::TEAM,
                                    __FILE__": executeV12() shoot_chance. angle=%.1f vel=(%.1f %.1f)%.2f"
                                    " update.",
                                    a, vel.x, vel.y, speed );
            }
        }

        if ( speed > max_speed )
        {

            max_speed = speed;
            best_angle = a;
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": executeV12() angle=%.1f vel=(%.1f %.1f)%.2f"
                                " update.",
                                a, vel.x, vel.y, speed );

        }
#ifdef DEBUG
        else
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": executeV12() angle=%.1f vel=(%.1f %.1f)%.2f"
                                " no update.",
                                a, vel.x, vel.y, speed );
        }
#endif
    }


    //
    // never accelerate the ball
    //

    if ( max_speed < 1.0 )
    {
        s_result = false;
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": failed executeV12. max_speed=%.2f", max_speed );
        return false;
    }

    //
    // execute tackle
    //

    s_result = true;
    s_best_angle = best_angle;

    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": executeV12() angle=%.0f", best_angle.degree() );
    agent->debugClient().addMessage( "BasicTackle%.0f", best_angle.degree() );
    agent->doTackle( ( best_angle - wm.self().body() ).degree() );
    agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );

    return true;
}
