// -*-c++-*-


/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_basic_move.h"

#include "block.h"
#include "markOn.h"
#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/server_param.h>

/*-------------------------------------------------------------------*/
/*!
  execute action
*/
bool
Bhv_BasicMove::execute( rcsc::PlayerAgent * agent )
{
	if ( Bhv_BasicTackle( 0.9, 80.0 ).execute( agent ) )
    {
        return true;
    }

    const rcsc::WorldModel & wm = agent->world();

    // check ball owner

    int self_min = wm.interceptTable()->selfReachCycle();
    int mate_min = wm.interceptTable()->teammateReachCycle();
    int opp_min = wm.interceptTable()->opponentReachCycle();

    if ( ! wm.existKickableTeammate()
         && ( self_min <= 3
              || ( self_min < mate_min + 3
                   && self_min < opp_min + 4 )
              )
         )
    {
        rcsc::Body_Intercept().execute( agent );
        if ( wm.ball().distFromSelf()
             < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        return true;
    }

  if(MarkOn().execute(agent))
    return true ;
	//block

	if(Block().execute(agent))
	    return true;
	    
	// positioning
	

	
    // go to home position
	
    double dash_power ;
	if(std::fabs(wm.ball().pos().x)>32.5)
		dash_power=(rcsc::ServerParam::i().maxDashPower());
	else
		dash_power = getDashPower( agent, M_home_pos )*1.3;
    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    if ( ! rcsc::Body_GoToPoint( M_home_pos, dist_thr, dash_power
                                 ).execute( agent ) )
    {
        rcsc::Body_TurnToBall().execute( agent );
    }

    if ( wm.existKickableOpponent()
         && wm.ball().distFromSelf() < 18.0 )
    {
        agent->setNeckAction( new rcsc::Neck_TurnToBall() );
    }
    else
    {
        agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
    }

    return true;

}
/*-------------------------------------------------------------------*/
/*!

*/
double
Bhv_BasicMove::getDashPower( const rcsc::PlayerAgent * agent,
                             const rcsc::Vector2D & target_point )
{
    static bool s_recover_mode = false;

    const rcsc::WorldModel & wm = agent->world();
    /*--------------------------------------------------------*/
    // check recover

    if ( wm.self().stamina() < rcsc::ServerParam::i().staminaMax() * 0.5 )
    {
        s_recover_mode = true;
    }
    else if ( wm.self().stamina() > rcsc::ServerParam::i().staminaMax() * 0.7 )
    {
        s_recover_mode = false;
    }

    /*--------------------------------------------------------*/
    double dash_power = rcsc::ServerParam::i().maxDashPower();
    const double my_inc
        = wm.self().playerType().staminaIncMax()
        * wm.self().recovery();

    int self_min = wm.interceptTable()->selfReachCycle();
    int mate_min = wm.interceptTable()->teammateReachCycle();
    int opp_min = wm.interceptTable()->opponentReachCycle();

    if ( wm.defenseLineX() > wm.self().pos().x
         && wm.ball().pos().x < wm.defenseLineX() + 20.0 )
    {
        // keep max power
        dash_power = rcsc::ServerParam::i().maxDashPower();
    }
    else if ( ! wm.existKickableTeammate()
              && opp_min < self_min
              && opp_min < mate_min
              && target_point.x < wm.self().pos().x
              && target_point.x < -25.0
              && wm.ball().inertiaPoint( opp_min ).x < -25.0 )
    {
        dash_power = rcsc::ServerParam::i().maxDashPower();
    }

    else if ( s_recover_mode )
    {
        dash_power = my_inc - 25.0; // preffered recover value
        if ( dash_power < 0.0 ) dash_power = 0.0;

    }
    // exist kickable teammate
    else if ( wm.existKickableTeammate()
              && wm.ball().distFromSelf() < 20.0 )
    {
        dash_power = std::min( my_inc * 1.1,
                               rcsc::ServerParam::i().maxDashPower() );
    }
    // in offside area
    else if ( wm.self().pos().x > wm.offsideLineX() )
    {
        dash_power = rcsc::ServerParam::i().maxDashPower();
    }
    // normal
    else
    {
        dash_power = std::min( my_inc * 1.7,
                               rcsc::ServerParam::i().maxDashPower() );
    }

    return dash_power;
}
