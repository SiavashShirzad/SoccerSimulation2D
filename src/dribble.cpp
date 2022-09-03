// -*-c++-*-

/*
 *Copyright:

        S.Pegasus

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_basic_offensive_kick.h"

#include "body_kick_to_corner.h"
#include <rcsc/action/body_kick_one_step.h>
//#include <rcsc/action/body_advance_ball.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
//#include <rcsc/action/body_pass.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/action/shoot_table2008.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/sector_2d.h>
#include "dribble.h"
#include <vector>
#include <rcsc/geom/angle_deg.h>
#include <rcsc/player/say_message_builder.h>
using namespace rcsc;
using namespace std;
/*-------------------------------------------------------------------*/



Vector2D
Dribble::target(PlayerAgent * agent)
{
    const rcsc::WorldModel & wm = agent->world();
    Vector2D mdribble_point;
    vector <double> mdribble_angle;
    vector <double> mdribble_angle1;
    Circle2D trakom1( wm.self().pos(),7.0);
    if ( wm.countOpponentsIn(trakom1,5,false) > 3)
        return Vector2D (1110,1110);
    AngleDeg best_angle = (wm.self().pos()-Vector2D(52.5,0)).th();
    if ( wm.ball().pos().y > 5 )
      AngleDeg best_angle = (wm.self().pos()-Vector2D(52.5,33)).th();
    if ( wm.ball().pos().y < -5 )
      AngleDeg best_angle = (wm.self().pos()-Vector2D(52.5,-33)).th();
    double d=best_angle.abs();
    for (double angle = -90 ; angle <= 90 ;angle += 10.0)
    {
        Sector2D sec(wm.ball().pos(),0.0,20,angle - 25.0,angle + 25.0);
        if ( ! wm.existOpponentIn( sec, 10, true ) )
            mdribble_angle1.push_back(angle);
    }
    if (mdribble_angle1.empty())
    {
      AngleDeg best_angle = (wm.self().pos()-Vector2D(52.5,0)).th();
      double d=best_angle.abs();
    }
    else
    {
      for (size_t i=0;i<mdribble_angle1.size();i++)
      {
	  for (size_t j=i+1;j<mdribble_angle1.size();j++)
	  {
	      if ((mdribble_angle1[i]-(d)) > (mdribble_angle1[j]-(d)))
	      {
		  double change = mdribble_angle1[i];
		  mdribble_angle1[i] = mdribble_angle1 [j];
		  mdribble_angle1[j] = change;
	      }
	  }
      }
      d = mdribble_angle1[0];
    }
    for (double angle = 0.0 ; angle < 360.0 ;angle += 20.0)
    {
        Sector2D sec(wm.ball().pos(),0.0,10,angle - 25.0,angle + 25.0);
        if ( wm.self().pos().x > -30 && wm.self().pos().x< 30)
            if ( angle > 90 && angle < 270 )
                continue;
        if ( ! wm.existOpponentIn( sec, 10, true ) )
            mdribble_angle.push_back(angle);
    }
    if ( mdribble_angle.empty() )
        return Vector2D (1110,1110);
    vector<Vector2D>mdribble_points;
    for (size_t i=0;i<mdribble_angle.size();i++)
    {
        for (size_t j=i+1;j<mdribble_angle.size();j++)
        {
            if ((mdribble_angle[i]-(d)) > (mdribble_angle[j]-(d)))
            {
                double change = mdribble_angle[i];
                mdribble_angle[i] = mdribble_angle [j];
                mdribble_angle[j] = change;
            }
        }
    }
    for (int i=0;i<7;i++)
    {
        mdribble_points.push_back(wm.self().pos() + rcsc::Vector2D::polar2vector(10.0,mdribble_angle[i]));
    }
    Vector2D dribblePoint (-1000,-1000);
    for(size_t i =0;i<mdribble_points.size();i++)
    {
      if(mdribble_points[i].x > dribblePoint.x)
	dribblePoint = mdribble_points[i];
    }
    return dribblePoint;

}
bool
Dribble::execute(rcsc::PlayerAgent * agent )
{
		
    const rcsc::WorldModel & wm = agent->world();
    Circle2D circ (wm.self().pos(),2);
    if(wm.existOpponentIn(circ , 1 , true))
      return false;
    Vector2D mdribble_point;
    mdribble_point = target(agent);
    if (mdribble_point.x > 1000 )
        return false;
    if ( mdribble_point.absX() > 50 || mdribble_point.absY() > 32 )
        return false ;
    Circle2D trakom( mdribble_point,7.0);
    if ( wm.countOpponentsIn(trakom,5,false) > 3)
        return false;
    double dash = rcsc::ServerParam::i().maxDashPower();
    if ( mdribble_point.x >-20 )
        dash = rcsc::ServerParam::i().maxDashPower() ;
    else
        if ( mdribble_point.x >20 )
            dash = rcsc::ServerParam::i().maxDashPower() ;
        else
            dash = rcsc::ServerParam::i().maxDashPower() ;

    if ( rcsc::Body_Dribble( mdribble_point,
                             1.0,
                             dash,
                             2
                           ).execute( agent ))
    {
        agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
        std::cout<<"dribble"<<std::endl;
        return true;
    }

    return false;
}
