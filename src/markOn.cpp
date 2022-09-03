
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <rcsc/player/debug_client.h>
#include <rcsc/action/neck_turn_to_ball.h>
#include <rcsc/action/neck_turn_to_ball_and_player.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include "bhv_basic_move.h"
#include <rcsc/player/player_agent.h>
#include <rcsc/geom/sector_2d.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/vector_2d.h>
#include <vector>
#include "markOn.h"
using namespace rcsc;
using namespace std;



bool MarkOn::execute(rcsc::PlayerAgent * agent)
{
	const rcsc::WorldModel & wm=agent->world();

	if( wm.self().stamina() < rcsc::ServerParam::i().staminaMax() * 0.4 )
		return false;
	if (wm.self().pos().x > -12)
		return false;
	if(wm.self().unum()==1)
		return false;
	if(wm.self().isKickable())
		return false;
		
			bool found = false;
			rcsc::PlayerObject * opp;
			const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
			const rcsc::PlayerPtrCont::const_iterator opps_end = opps.end();
			for ( rcsc::PlayerPtrCont::const_iterator it = opps.begin() ; it != opps_end ; ++it )
			{
				if ((*it)->pos().x > -35.0)
					continue;
				if ((*it)->posCount() > 4)
					continue;
				if ((*it)->isKickable())
					continue;
//				if((*it)->pos().x > -35 || (*it)->pos().absY()>14 )
//					continue;
				bool not_this_opp = false;
				const rcsc::PlayerPtrCont & tms = wm.teammatesFromSelf();
				const rcsc::PlayerPtrCont::const_iterator tms_end = tms.end();
				for ( rcsc::PlayerPtrCont::const_iterator tm = tms.begin() ; tm != tms_end ; ++tm )
				{
					if((*tm)->pos().dist( (*it)->pos() ) < wm.self().pos().dist( (*it)->pos() ))
					{
						not_this_opp = true;
						break;
					}
				}
				if( not_this_opp )
					continue;
				opp=*it;
				found=true;
				}
				
		if(found)
			{
				Vector2D a ((opp->pos().x) - 1 , opp->pos().y);
				double dashPower=rcsc::ServerParam::i().maxDashPower();
				rcsc::Body_GoToPoint(a,0.5,dashPower).execute(agent);
				if(wm.self().distFromBall()>25)
				{
				  rcsc::Neck_TurnToLowConfTeammate().execute(agent);
				  agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
				}
				else
				{
				  rcsc::Neck_TurnToBallAndPlayer(opp,0).execute(agent);
				  agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
				}
				//std::cout<<"Mark executed for player  "<<wm.self().unum()<<" to mark opponent number  "<<opp->unum()<<std::endl;
				agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
				return true;
			}
			else
				return false;
		}
				
	
