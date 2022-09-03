// -*-c++-*-

/*
 *Copyright:

 mojtaba jafari
 *

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "block.h"
#include "body_kick_to_corner.h"

#include <rcsc/action/body_intercept.h>
#include <rcsc/action/body_advance_ball.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/body_turn_to_ball.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_ball.h>

#include <rcsc/player/audio_sensor.h>
#include <rcsc/player/say_message_builder.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/sector_2d.h>
#include <rcsc/geom/rect_2d.h>
#include <vector>
#include "bhv_predictor.h"

using namespace rcsc;
using namespace std;

rcsc::Rect2D danger( rcsc::Vector2D(-52.5,-20.0),rcsc::Vector2D(-36.0,20.0));
bool
Block::execute( rcsc::PlayerAgent * agent )
{
 // if(iTeamCycles )

    const rcsc::WorldModel & wm = agent->world();
    vector<Vector2D>blockable;
    const AbstractPlayerCont::const_iterator
    t_end = wm.allTeammates().end();
    AbstractPlayerCont::const_iterator
    it = wm.allTeammates().begin();

    std::vector< std::pair< const rcsc::AbstractPlayerObject * , double > >teammates;
    if(wm.self().unum()==10||wm.self().unum()==9||wm.self().unum()==11)
    return false;
///////////////////////////////////////////////////////////////////////////////////////
  
    
/*    
    const PlayerObject * notb = wm.getOpponentNearestToBall(8);
    const PlayerObject * nttb = wm.getTeammateNearestToBall(8);
    
    if( notb && nttb && notb->distFromBall()>5.0 && nttb->unum()==wm.self().unum())
    {
        rcsc::Body_Intercept().execute( agent );
        if ( wm.ball().distFromSelf() < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        cout<<wm.self().unum()<<" blocker intercept, man injama?!"<<endl;


        return true; 
    }
    
    
*/    
    
    
///////////////////////////////////////////////////////////////////////////////////////
    
    
    double dist = 2.4;
    rcsc::Line2D goalline( wm.ball().pos(),Vector2D(-52.5,0) );
    rcsc::Line2D Xline ( rcsc::Vector2D( wm.ball().pos().x - dist , 34 ) , rcsc::Vector2D( wm.ball().pos().x - dist , -34 )) ;
    rcsc::Vector2D inter = goalline.intersection(Xline);


    for ( ; it != t_end ; ++it )
    {
        if ( (*it)->pos().x > wm.ball().pos().x + 8.0 )
            continue;
        if ( std::fabs( (*it)->pos().y - wm.ball().pos().y ) > 15.0 )
            continue;

        double value = 1000;
        value /= ((*it)->pos().dist(inter));

        teammates.push_back( std::make_pair( *it , value ) );
    }

    if ( teammates.empty() )
        return false;

    const rcsc::AbstractPlayerObject* temp ;
    double value ;
    for (unsigned int i=0;i<teammates.size()-1;i++)
    {
        for (unsigned int j=i+1;j<teammates.size();j++)
        {
            if (teammates[i].second < teammates[j].second)
            {
                temp = teammates[i].first;
                teammates[i].first = teammates[j].first ;
                teammates[j].first = temp ;
                value = teammates[i].second ;
                teammates[i].second = teammates[j].second ;
                teammates[j].second = value ;
            }
        }
    }


    Vector2D block_pos = inter;
    static int counter=0;
    if ( wm.self().pos().dist( teammates[0].first->pos() ) > 1.0 )
    {
        return false;
    }

    const double dash_power = rcsc::ServerParam::i().maxDashPower();
    double dist_thr = 2.1;



    Vector2D p1 ;
    Vector2D p2 ;
    rcsc::Rect2D danger( rcsc::Vector2D(-52.5,-20.0),rcsc::Vector2D(-36.0,20.0));
    
    if(! danger.contains(wm.ball().pos()))
    if(teammates[0].first->unum()==2||teammates[0].first->unum()==3||teammates[0].first->unum()==4||teammates[0].first->unum()==5)
    {
     p1=teammates[0].first->pos() + Vector2D(-3,-4);
     p2=teammates[0].first->pos() + Vector2D( 7, 4);
     rcsc::Rect2D rec1( p1,p2);
     if(!rec1.contains(wm.ball().pos()))
       return false ;
    }
 

//   if ( wm.ball().pos().x < -27.0)
//{
    if ( danger.contains(wm.ball().pos()))
    {
        if (rcsc::Body_GoToPoint( block_pos, dist_thr, dash_power, -1.0, 100, false , 60.0).execute( agent ) )
    {
        counter=0;
                if ( wm.ball().distFromSelf() < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        cout<<wm.self().unum()<<" blocker "<<endl;
        if ( agent->config().useCommunication())
        {
         agent->addSayMessage( new PassMessage( wm.self().unum(),
                                                   block_pos,// + target_buf,
                                                   agent->effector().queuedNextBallPos(),
                                                   agent->effector().queuedNextBallVel() ) );
        }
        return true;
    }
    }
//}

    if (rcsc::Body_GoToPoint( block_pos, dist_thr, dash_power, -1.0, 100, false , 60.0).execute( agent ) )
    {
        counter=0;
                if ( wm.ball().distFromSelf() < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        cout<<wm.self().unum()<<" blocker "<<endl;
        if ( agent->config().useCommunication())
        {
         agent->addSayMessage( new PassMessage( wm.self().unum(),
                                                   block_pos,// + target_buf,
                                                   agent->effector().queuedNextBallPos(),
                                                   agent->effector().queuedNextBallVel() ) );
        }
        return true;
    }



    counter++;
    if (counter==1)
    {
        rcsc::Body_Intercept().execute( agent );
        counter=0;
        if ( wm.ball().distFromSelf() < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        cout<<wm.self().unum()<<" blocker intercept, man injama?!"<<endl;


        return true;
    }
    cout<<wm.self().unum()<<" block false"<<endl;
    return false ;
}
