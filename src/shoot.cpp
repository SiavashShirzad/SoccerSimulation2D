//SIDIOUS
		#ifdef HAVE_CONFIG_H
		#include <config.h>
		#endif

		#include "shoot.h"
		#include "bhv_predictor.h"
		#include "body_kick_to_corner.h"
		#include <rcsc/player/say_message_builder.h>
		#include <rcsc/action/shoot_table2008.h>
		//#include <rcsc/action/body_advance_ball.h>
		#include <rcsc/action/body_dribble.h>
		#include <rcsc/action/body_hold_ball.h>
		#include <rcsc/action/neck_scan_field.h>
		#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
		#include <rcsc/action/body_smart_kick.h>	
		#include <rcsc/player/player_agent.h>
		#include <rcsc/player/debug_client.h>
		#include <vector>
		#include <iostream>
		#include <rcsc/common/logger.h>
		#include <rcsc/common/server_param.h>
		#include <rcsc/geom/sector_2d.h>
		#include <rcsc/action/body_kick_one_step.h>
		#include <rcsc/geom/circle_2d.h>
		#include <rcsc/geom/triangle_2d.h>
		using namespace std;
		using namespace rcsc;	
		/*-------------------------------------------------------------------*/
		
		bool Shoot::execute( rcsc::PlayerAgent * agent )
		{ 
						if( get_Shootpoint( agent, &mShootpoint ) == false )
				return false;
			
			doShoot( agent , mShootpoint );
		 
			return true;

			
}
bool Shoot::doShoot(PlayerAgent * agent , Vector2D Shoot )
		{
		const rcsc::WorldModel & wm = agent->world();	
		int kick_step = ( agent->world().gameMode().type() != GameMode::PlayOn
							  && agent->world().gameMode().type() != GameMode::GoalKick_
							  ? 1
							  : 3 );
			double end_speed = 3.0;
			double first_speed = 20.0;
			do
			{
				first_speed
					= calc_first_term_geom_series_last
					( end_speed,
					  Shoot.dist(wm.ball().pos()),
					  ServerParam::i().ballDecay() );
				//= required_first_speed( receiver_dist,
				//ServerParam::i().ballDecay(),
				//end_speed );
				if ( first_speed < ServerParam::i().ballSpeedMax() )
				{
					break;
				}
				end_speed -= 0.07;             
			}
			while ( end_speed > 0.8 );

			if ( ! Body_SmartKick( Shoot,
								   first_speed,
								   first_speed * 0.96,
								   kick_step ).execute( agent ) )
			{
				if ( agent->world().gameMode().type() != GameMode::PlayOn
					 && agent->world().gameMode().type() != GameMode::GoalKick_ )
				{
					first_speed =  ServerParam::i().maxPower() ;
											
					Body_KickOneStep( Shoot,									  first_speed
									  ).execute( agent );
					agent->setNeckAction( new rcsc::Neck_TurnToLowConfTeammate() );
				}
				else
				{
								return false;
				}
			}
			if ( agent->config().useCommunication() )
			{
						Vector2D target_buf = Shoot - agent->world().ball().pos();
				target_buf.setLength( 1.0 );

				agent->addSayMessage( new PassMessage(mUnum,
													
													  Shoot + target_buf,
													
													agent->effector().queuedNextBallPos(),
			
														agent->effector().queuedNextBallVel() ) );
			
			std::cout<<"SHOOT";
			
			}
			return true;
		}
bool Shoot::get_Shootpoint(PlayerAgent * agent,Vector2D * Shoot)
{
	
	const rcsc::WorldModel & wm = agent->world();
	vector<pair<Vector2D,double> > shootpoints;
	if(wm.self().pos().x < .0 || wm.self().pos().absY() >14.0 )
				return false;
				int oppcycle;
				int ballcycle;
				for(double i= -6.5;i <6.5 ;i=i + 0.3)
				{
					
					Vector2D Shootpoints (52.5 , i);
					if(!canShoot( agent , &Shootpoints , &oppcycle , &ballcycle))
					continue;
					
					shootpoints.push_back(make_pair(Shootpoints,0.0));
					}
					cout << "ssssssssssssssssssssssssssss"<<shootpoints.size();
					if(shootpoints.size() < 1)
					return false;
				for(size_t i=0;i<shootpoints.size();i++)
		{
			//shootpoints[i].second += 100* Shootpoints.absY();
			shootpoints[i].second += 100*(oppcycle-ballcycle);
			}
			double max = -10000;
	
		for(size_t i=0;i < shootpoints.size();i++)
		{
			if (shootpoints[i].second > max)
			{
				max = shootpoints[i].second;
				*Shoot=shootpoints[i].first;			
			}
		}
		
		return true;
	
	}
	bool Shoot::canShoot(PlayerAgent * agent ,Vector2D *Shootpoints , int * oppcycle, int *ballcycle)
	{
		const rcsc::WorldModel & wm = agent->world();
const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
const PlayerPtrCont::const_iterator opps_end = 	opps.end();
				
                for ( PlayerPtrCont::const_iterator that = opps.begin();
                  that != opps_end;
                  ++that )   
				{
					if ((*that)->posCount() > 5)
					continue;
					rcsc::Line2D line(wm.ball().pos() , *Shootpoints);
					rcsc::Line2D prend = line.perpendicular((*that)->pos());
					const rcsc::Vector2D target_point = prend.intersection(line);
				
					* oppcycle = Bhv_Predictor::predict_player_reach_cycle ( (*that) , target_point , 1.0,1.0,3,1,1,false);
					double end_speed = 3.0;
				    double first_speed = 20.0;
				    do
				    {
				        first_speed
				            = calc_first_term_geom_series_last
				            ( end_speed,
				              (*Shootpoints).dist(wm.ball().pos()),
				              ServerParam::i().ballDecay() );
				        if ( first_speed < ServerParam::i().ballSpeedMax() )
				        {
				            break;
				        }
						end_speed -= 0.09;
				    }
				    while ( end_speed > 0.4 );
					
					* ballcycle = (int) rcsc::calc_length_geom_series( first_speed,
                                   wm.ball().pos().dist( target_point ),
                                   rcsc::ServerParam::i().ballDecay() );
					if(*oppcycle<*ballcycle+3)	

						{
							return false;
						}
				}
		return true;
					
		}

