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

#include "bhv_basic_offensive_kick.h"
#include "shoot.h"

#include "dribble.h"

#include "body_kick_to_corner.h"
#include "pass.h"
#include "throughpass1.h"
#include <rcsc/action/body_advance_ball.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
//#include <rcsc/action/body_pass.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/sector_2d.h>
#include <iostream>
/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_BasicOffensiveKick::execute( rcsc::PlayerAgent * agent )
{
    rcsc::dlog.addText( rcsc::Logger::TEAM,
                        __FILE__": Bhv_BasicOffensiveKick" );

    const rcsc::WorldModel & wm = agent->world();

    const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
    const rcsc::PlayerObject * nearest_opp
        = ( opps.empty()
            ? static_cast< rcsc::PlayerObject * >( 0 )
            : opps.front() );
    const double nearest_opp_dist = ( nearest_opp
                                      ? nearest_opp->distFromSelf()
                                      : 1000.0 );
    const rcsc::Vector2D nearest_opp_pos = ( nearest_opp
                                             ? nearest_opp->pos()
                                             : rcsc::Vector2D( -1000.0, 0.0 ) );
rcsc::Vector2D tmp(-1000,1000);
Circle2D dangerCircle (wm.self().pos(),5);
if( Shoot().get_Shootpoint( agent , &tmp ) )
	{
			Shoot().execute(agent);
	
			return true;
	}
	

   if( ThroughPass1().execute(agent))
		{
			agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
			return true;
		}
	
		if(wm.existOpponentIn(dangerCircle,4,false))
		{
		  Pass pass;
		  if(pass.execute(agent))
		    return true;
		}
    Sector2D sec(wm.ball().pos(),0.0,10,-30,30);
    Circle2D cir(wm.self().pos(),5);
		if(!wm.existOpponentIn(sec , 2 , false)&& !wm.existOpponentIn(cir , 2 , false))
		{
		  Dribble dribble;
		  if(dribble.execute(agent))
		    return true;
		}

	
	
/*	    

    rcsc::Vector2D pass_point;
    if ( rcsc::Body_Pass::get_best_pass( wm, &pass_point, NULL, NULL ) )
    {
        if ( pass_point.x > wm.self().pos().x - 1.0 )
        {
            bool safety = true;
            const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
            for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin();
                  it != opps_end;
                  ++it )
            {
                if ( (*it)->pos().dist( pass_point ) < 4.0 )
                {
                    safety = false;
                }
            }

            if ( safety )
            {
                rcsc::dlog.addText( rcsc::Logger::TEAM,
                                    __FILE__": (execute) do best pass" );
                agent->debugClient().addMessage( "OffKickPass(1)" );
                rcsc::Body_Pass().execute( agent );
                agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
                return true;
            }
        }
    }*/
if ( nearest_opp_dist < 7.0 )
    {
     /*   if ( rcsc::Body_Pass().execute( agent ) )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": (execute) do best pass" );
            agent->debugClient().addMessage( "OffKickPass(2)" );
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
            return true;PASS*/
            rcsc::Vector2D tmp(-1000,1000);
	if( Pass().get_Passpoint( agent , &tmp ) )
	{
		if( tmp.x > wm.self().pos().x - 15.0 )
		{
			Pass().execute(agent);
			agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
			return true;
		}
	}
        }
    

    // dribble to my body dir
    if ( nearest_opp_dist < 5.0
         && nearest_opp_dist > ( rcsc::ServerParam::i().tackleDist()
                                 + rcsc::ServerParam::i().defaultPlayerSpeedMax() * 1.5 )
         && wm.self().body().abs() < 70.0 )
    {
        const rcsc::Vector2D body_dir_drib_target
            = wm.self().pos()
            + rcsc::Vector2D::polar2vector(5.0, wm.self().body());

        int max_dir_count = 0;
        wm.dirRangeCount( wm.self().body(), 20.0, &max_dir_count, NULL, NULL );

        if ( body_dir_drib_target.x < rcsc::ServerParam::i().pitchHalfLength() - 1.0
             && body_dir_drib_target.absY() < rcsc::ServerParam::i().pitchHalfWidth() - 1.0
             && max_dir_count < 3
             )
        {
            // check opponents
            // 10m, +-30 degree
            const rcsc::Sector2D sector( wm.self().pos(),
                                         0.5, 10.0,
                                         wm.self().body() - 30.0,
                                         wm.self().body() + 30.0 );
            // opponent check with goalie
            if ( ! wm.existOpponentIn( sector, 10, true ) )
            {
                rcsc::dlog.addText( rcsc::Logger::TEAM,
                                    __FILE__": (execute) dribble to my body dir" );
                agent->debugClient().addMessage( "OffKickDrib(1)" );
                rcsc::Body_Dribble( body_dir_drib_target,
                                    1.0,
                                    rcsc::ServerParam::i().maxDashPower(),
                                    2
                                    ).execute( agent );
                agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
                return true;
            }
        }
    }

    rcsc::Vector2D drib_target( 50.0, wm.self().pos().absY() );
    if ( drib_target.y < 20.0 ) drib_target.y = 20.0;
    if ( drib_target.y > 29.0 ) drib_target.y = 27.0;
    if ( wm.self().pos().y < 0.0 ) drib_target.y *= -1.0;
    const rcsc::AngleDeg drib_angle = ( drib_target - wm.self().pos() ).th();

    // opponent is behind of me
    if ( nearest_opp_pos.x < wm.self().pos().x + 1.0 )
    {
        // check opponents
        // 15m, +-30 degree
        const rcsc::Sector2D sector( wm.self().pos(),
                                     0.5, 15.0,
                                     drib_angle - 30.0,
                                     drib_angle + 30.0 );
        // opponent check with goalie
        if ( ! wm.existOpponentIn( sector, 10, true ) )
        {
            const int max_dash_step
                = wm.self().playerType()
                .cyclesToReachDistance( wm.self().pos().dist( drib_target ) );
            if ( wm.self().pos().x > 35.0 )
            {
                drib_target.y *= ( 10.0 / drib_target.absY() );
            }

            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": (execute) fast dribble to (%.1f, %.1f) max_step=%d",
                                drib_target.x, drib_target.y,
                                max_dash_step );
            agent->debugClient().addMessage( "OffKickDrib(2)" );
            rcsc::Body_Dribble( drib_target,
                                1.0,
                                rcsc::ServerParam::i().maxDashPower(),
                                std::min( 5, max_dash_step )
                                ).execute( agent );
        }
        else
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": (execute) slow dribble to (%.1f, %.1f)",
                                drib_target.x, drib_target.y );
            agent->debugClient().addMessage( "OffKickDrib(3)" );
            rcsc::Body_Dribble( drib_target,
                                1.0,
                                rcsc::ServerParam::i().maxDashPower(),
                                2
                                ).execute( agent );

        }
        agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
        return true;
    }

    // opp is far from me
    if ( nearest_opp_dist > 5.0 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": opp far. dribble(%.1f, %.1f)",
                            drib_target.x, drib_target.y );
        agent->debugClient().addMessage( "OffKickDrib(4)" );
        rcsc::Body_Dribble( drib_target,
                            1.0,
                            rcsc::ServerParam::i().maxDashPower() * 0.4,
                            1
                            ).execute( agent );
        agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
        return true;
    }


if( Shoot().get_Shootpoint( agent , &tmp ) )
	{
			Shoot().execute(agent);
			return true;
	}
		
		if(wm.existOpponentIn(dangerCircle,1,false))
		{
		  Pass pass;
		  if(pass.execute(agent))
		    return true;
		}
    // opp is far from me
    if ( nearest_opp_dist > 3.0 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": (execute) opp far. dribble(%f, %f)",
                            drib_target.x, drib_target.y );
        agent->debugClient().addMessage( "OffKickDrib(5)" );
        rcsc::Body_Dribble( drib_target,
                            1.0,
                            rcsc::ServerParam::i().maxDashPower() * 0.2,
                            1
                            ).execute( agent );
        agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
        return true;
    }

    if ( nearest_opp_dist > 2.5 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": hold" );
        agent->debugClient().addMessage( "OffKickHold" );
        rcsc::Body_HoldBall().execute( agent );
        agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
        return true;
    }

    if ( wm.self().pos().x > wm.offsideLineX() - 10.0 )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": kick near side" );
        agent->debugClient().addMessage( "OffKickToCorner" );
        Body_KickToCorner( (wm.self().pos().y < 0.0) ).execute( agent );
        agent->setNeckAction( new rcsc::Neck_ScanField() );
    }
    else
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": clear" );
        agent->debugClient().addMessage( "OffKickAdvance" );
//  rcsc::Body_AdvanceBall().execute( agent );
        agent->setNeckAction( new rcsc::Neck_ScanField() );
    }

    return true;
}
