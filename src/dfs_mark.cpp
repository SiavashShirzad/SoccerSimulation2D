// -*-c++-*-

/*
Copyright:


 TORNADO


EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#include "dfs_mark.h"

#include "bhv_basic_move.h"
#include <rcsc/player/player_agent.h>
#include <rcsc/player/world_model.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_ball.h>
#include <rcsc/action/neck_turn_to_ball_and_player.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/geom/vector_2d.h>
#include <rcsc/geom/sector_2d.h>
#include <rcsc/common/server_param.h>

Mark::Mark(PlayerAgent * agentArg) : DefensiveAction ( agentArg )
{

}

Mark::~Mark()
{

}

Vector2D Mark::getMarkPoint()
{
    return targetPoint;
}

PlayOffMark::PlayOffMark(PlayerAgent * agentArg) : Mark(agentArg)
{

}

PlayOffMark::~PlayOffMark()
{

}
bool PlayOffMark::shouldMark()
{
	if( wm->self().stamina() < rcsc::ServerParam::i().staminaMax() * 0.4 )
		return false;

	//if(wm->existKickableTeammate())
    //    return false;

		if((wm->self().pos()).dist(wm->ball().pos())>30)
			return false;
		else
		{
			const rcsc::PlayerPtrCont & opps = wm->opponentsFromSelf();
			const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
			for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin() ; it != opps_end ; ++it )
			{
				if((*it)->distFromBall()>30)
					continue;
				if((*it)->distFromBall()<5)
					continue;
				if((*it)->goalie())
					continue;
				if((*it)->posCount()>8)
					continue;
				Sector2D sec1(wm->ball().pos(),0.0,(wm->ball().pos()-(*it)->pos()).r(),(wm->ball().pos()-(*it)->pos()).th()-10,(wm->ball().pos()-(*it)->pos()).th()+10);
				if(wm->existTeammateIn(sec1,10,true))
					continue;
				bool not_this_opp = false;

                const rcsc::PlayerObject * NearestTm = wm-> getTeammateNearestTo ((*it),8,NULL);
                const rcsc::PlayerObject * NearestOppToTm = wm-> getOpponentNearestTo (NearestTm,8,NULL);

                if ( NearestTm && NearestOppToTm && NearestOppToTm == NearestTm)
                    not_this_opp = true;
				if( not_this_opp )
					continue;
				targetPoint = (*it)->pos();
				return true;
			}
			return false;
		}

}

bool PlayOffMark::execute()
{
                double dashPower=Bhv_BasicMove::getDashPower(agent,targetPoint)*1.5;
				rcsc::Body_GoToPoint(targetPoint,0.5,dashPower).execute(agent);


				//std::cout<<"Mark executed for player  "<<wm->self().unum()<<" to mark opponent number  "<<opp->unum()<<std::endl;
				agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
				return true;
}

PlayOnMark::PlayOnMark(PlayerAgent * agentArg) : Mark(agentArg)
{

}

PlayOnMark::~PlayOnMark()
{

}

Rect2D PlayOnMark::getMarkRect()
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


bool PlayOnMark::shouldMark()
{
    return true;
}

bool PlayOnMark::execute()
{
    return true;
}
