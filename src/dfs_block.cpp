// -*-c++-*-

/*
Copyright:


 Mojtaba Jafari


EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#include "dfs_block.h"

#include "defensive_action.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/player/world_model.h>

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
#include <rcsc/geom/circle_2d.h>
#include <vector>

///////////////////////////////////
extern void checkLogger(PlayerAgent *);
extern void logText(const char *);
extern void logVector(rcsc::Vector2D vec);
////////////////////////////////////

using namespace rcsc;
using namespace std;

Block::Block (PlayerAgent * agentArg) : DefensiveAction (agentArg)
{

}

Block::~Block()
{

}

Rect2D Block::getBlockRect()
{

  int u_num = wm->self().unum();

  rcsc::Vector2D topLeft;
  rcsc::Vector2D bottomRight;
  int upperTmm=0;
  int lowerTmm=0;
  double upperTmmY;
  double lowerTmmY;
  switch (u_num)
  {
    case 2:
      upperTmm = 4;
      break;
    case 3:
      upperTmm = 2;
      break;
    case 5:
      upperTmm = 3;
      break;
    case 6:
      upperTmm = 7;
      break;
    case 8:
      upperTmm = 6;
      break;
    case 11:
      upperTmm = 9;
      break;
    case 10:
      upperTmm = 11;
      break;
    default:
      upperTmmY = -34.0;
  }
  switch (u_num)
  {
    case 4:
      lowerTmm = 2;
      break;
    case 2:
      lowerTmm = 3;
      break;
    case 3:
      lowerTmm = 5;
      break;
    case 7:
      lowerTmm = 6;
      break;
    case 6:
      lowerTmm = 8;
      break;
    case 9:
      lowerTmm = 11;
      break;
    case 11:
      lowerTmm = 10;
      break;
    default:
      lowerTmmY = 34.0;
  }

  if(!upperTmm == 0 || !lowerTmm == 0)
  {
    const rcsc::PlayerPtrCont & tms = wm->teammatesFromSelf();
    const rcsc::PlayerPtrCont::const_iterator tms_end = tms.end();
    for ( rcsc::PlayerPtrCont::const_iterator tm = tms.begin() ; tm != tms_end ; ++tm )
    {
      if((*tm)->unum() == upperTmm)
	upperTmmY = (*tm)->pos().y;
      if((*tm)->unum() == lowerTmm)
	lowerTmmY = (*tm)->pos().y;
    }
  }

  topLeft.y = (upperTmmY + wm->self().pos().y)/2;
  bottomRight.y = (lowerTmmY + wm->self().pos().y)/2;

  if( u_num == 2 || u_num == 3 || u_num ==4 || u_num == 5)
  {
    topLeft.x = -52.0 ;
    bottomRight.x = 0.0 ;
  }

  if( u_num == 9 || u_num == 11 || u_num ==10 )
  {
    topLeft.x = 0.0 ;
    bottomRight.x = 52.0 ;
  }

  if( u_num == 6 || u_num == 7 || u_num ==8 )
  {
    topLeft.x = -30.0 ;
    bottomRight.x = 30.0 ;
  }

 rcsc::Rect2D rect (topLeft , bottomRight);

 return rect;
}

bool Block::execute()
{
    vector<Vector2D>blockable;
    const AbstractPlayerCont::const_iterator    t_end = wm->allTeammates().end();
    AbstractPlayerCont::const_iterator          it = wm->allTeammates().begin();

    std::vector< std::pair< const rcsc::AbstractPlayerObject * , double > >teammates;

    double dist = 2.5;
    rcsc::Line2D goalline( wm->ball().pos(),Vector2D(-52.5,0) );
    rcsc::Line2D Xline ( rcsc::Vector2D( wm->ball().pos().x - dist , 34 ) , rcsc::Vector2D( wm->ball().pos().x - dist , -34 )) ;
    rcsc::Vector2D inter = goalline.intersection(Xline);


    for ( ; it != t_end ; ++it )
    {
        if ( (*it)->pos().x > wm->ball().pos().x + 8.0 )
            continue;
        if ( std::fabs( (*it)->pos().y - wm->ball().pos().y ) > 15.0 )
            continue;
        //////////////////////////////
        ////////////////
        double YdX;
        double deltaX = std::fabs( (*it)->pos().x - wm->ball().pos().x );
        double deltaY = std::fabs( (*it)->pos().y - wm->ball().pos().y );
        YdX = deltaY/deltaX ;
        if(deltaY>7.5 && YdX>1.8)
            continue;
        ///////////////
        ///////////////
       // /*
        const PlayerObject * nearestOppToTm = wm-> getOpponentNearestTo ((*it)->pos(),8,NULL);
        if( nearestOppToTm)
        {
            Vector2D oppPos = nearestOppToTm->pos();
            Circle2D cir(oppPos , 3.75);
            Sector2D sec(oppPos ,  0.0 , 5.5 , 100.0 , 260.0 );
            bool can_block = false;
            const rcsc::PlayerPtrCont & tms = wm->teammatesFromSelf();
            const rcsc::PlayerPtrCont::const_iterator tms_end = tms.end();
            for ( rcsc::PlayerPtrCont::const_iterator tm = tms.begin() ; tm != tms_end ; ++tm )
            {
                if(sec.contains((*tm)->pos()) || cir.contains((*tm)->pos()))
                {
                    can_block = true;
                    break;
                }
            }
            if(!can_block)
                continue;

        }
       // */
        /////////////
        ////////////////////////////

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
    if ( wm->self().pos().dist( teammates[0].first->pos() ) > 1.0 )
    {
        return false;
    }

    const double dash_power = rcsc::ServerParam::i().maxDashPower();
    double dist_thr = 2.2;


    rcsc::Rect2D danger( rcsc::Vector2D(-52.5,-20.0),rcsc::Vector2D(-36.0,20.0));
    Vector2D p1 ;
    Vector2D p2 ;

    if(! danger.contains(wm->ball().pos()))
    if(teammates[0].first->unum()==2||teammates[0].first->unum()==3||teammates[0].first->unum()==4||teammates[0].first->unum()==5)
    {
     //p1=teammates[0].first->pos() + Vector2D(-3,-4);
     //p2=teammates[0].first->pos() + Vector2D( 7, 4);
     //rcsc::Rect2D rec1( p1,p2);
     rcsc::Rect2D rec1 = getBlockRect();
     if(!rec1.contains(wm->ball().pos()))
       return false ;
    }




    if ( danger.contains(wm->ball().pos()))
    {
        rcsc::Body_Intercept().execute( agent );
        if ( wm->ball().distFromSelf() < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        cout<<wm->self().unum()<<" blocker intercept"<<endl;
        logText( "\n \t \t Danger contains the ball => Body_Intercept ");
        return true;
    }


    if (rcsc::Body_GoToPoint( block_pos, dist_thr, dash_power, -1.0, 100, false , 60.0).execute( agent ) )
    {
        counter=0;
                if ( wm->ball().distFromSelf() < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        cout<<wm->self().unum()<<" blocker "<<endl;
        if ( agent->config().useCommunication())
        {
         agent->addSayMessage( new PassMessage( wm->self().unum(),
                                                   block_pos,// + target_buf,
                                                   agent->effector().queuedNextBallPos(),
                                                   agent->effector().queuedNextBallVel() ) );
        }
        logText("\n \t \t Body_GoToPoint : ");
        logVector(block_pos);
        return true;

    }



    counter++;
    if (counter==1)
    {
        rcsc::Body_Intercept().execute( agent );
        counter=0;
        if ( wm->ball().distFromSelf() < rcsc::ServerParam::i().visibleDistance() )
        {
            agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );;
        }
        else
        {
            agent->setNeckAction( new rcsc::Neck_TurnToBallOrScan() );
        }
        cout<<wm->self().unum()<<" blocker intercept, man injama?!"<<endl;

        logText("\n \t \t counter = 1 => Body_Intercept ");
        return true;

    }
    cout<<wm->self().unum()<<" block false"<<endl;
    logText("\n \t \t Returned False");
    return false ;
}
