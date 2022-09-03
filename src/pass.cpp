		//siavash
		/////////////////////////////////////////////////////////////////////

		#ifdef HAVE_CONFIG_H
		#include <config.h>
		#endif

		#include "pass.h"
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
		#include <rcsc/common/logger.h>
		#include <rcsc/common/server_param.h>
		#include <rcsc/geom/sector_2d.h>
		#include <rcsc/action/body_kick_one_step.h>
		#include <rcsc/geom/circle_2d.h>
		#include <rcsc/geom/triangle_2d.h>
		using namespace std;	
		/*-------------------------------------------------------------------*/
		/*!

		*/
		bool
		Pass::execute( rcsc::PlayerAgent *  agent )
		{ 


			if( get_Passpoint( agent, &mpasspoint ) == false )
				return false;
			
			doPass( agent , mpasspoint );
		 
			return true;
		}

		/*kick ball and do pass*/ 
		/*..........................................................*/
		bool Pass::doPass(PlayerAgent * agent , Vector2D pass )
		{
		const rcsc::WorldModel & wm = agent->world();	
		int kick_step = ( agent->world().gameMode().type() != GameMode::PlayOn
							  && agent->world().gameMode().type() != GameMode::GoalKick_
							  ? 1
							  : 3 );
			double end_speed = 2.6;
			double first_speed = 20.0;
			do
			{
				first_speed
					= calc_first_term_geom_series_last
					( end_speed,
					  pass.dist(wm.ball().pos()),
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

			if ( ! Body_SmartKick( pass,
								   first_speed,
								   first_speed * 0.96,
								   kick_step ).execute( agent ) )
			{
				if ( agent->world().gameMode().type() != GameMode::PlayOn
					 && agent->world().gameMode().type() != GameMode::GoalKick_ )
				{
					first_speed = std::min( agent->world().self().kickRate() * ServerParam::i().maxPower(),
											first_speed );
					Body_KickOneStep( pass,
									  first_speed
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
						Vector2D target_buf = pass - agent->world().ball().pos();
				target_buf.setLength( 1.0 );

				agent->addSayMessage( new PassMessage(mUnum,
													
													   pass + target_buf,
													
													agent->effector().queuedNextBallPos(),
			
														agent->effector().queuedNextBallVel() ) );
			
			std::cout<<"pass"<<std::endl;
			
			}
			return true;
		}


//...........................

		bool Pass::get_Passpoint(PlayerAgent * agent,Vector2D * pass)

		{

		const rcsc::WorldModel & wm = agent->world();
			
			

		// loop candidate teammates


			const PlayerPtrCont::const_iterator
				t_end = wm.teammatesFromSelf().end();
			for ( PlayerPtrCont::const_iterator
					  it = wm.teammatesFromSelf().begin();
				  it != t_end;
				  ++it )


			{
				int oppcycle;
				Vector2D endpos = (*it)->pos();
						Line2D l(wm.ball().pos(),endpos);
							Line2D b(wm.ball().pos(),endpos);
						Vector2D inter = (*it)->pos();
Sector2D trans(wm.ball().pos(),1.0,wm.ball().pos().dist(inter),(inter - wm.ball().pos()).th() - 10.0,( inter-wm.ball().pos()).th() + 10.0);
			Circle2D cir((*it)->pos(),1);
			 if(wm.existOpponentIn(trans,3.0,true))
			continue;
					if(wm.existOpponentIn(cir,3.0,true))
			continue;
					if ( (*it)->goalie() )
			continue;
					if ( (*it)->pos().x > 52.0 || (*it)->pos().absY() > 34)
			continue;
					if ( (*it)->pos().x < -36.0 && (*it)->pos().absY() < 17)
			continue;
					if ( (*it)->pos().x > wm.offsideLineX()-0.5)
			continue;
					if ( (*it)->pos().x < wm.self().pos().x - 10.0)
			continue;
					if (( (*it)->pos().dist(wm.self().pos()) < 4.0))
			continue;
					if(!canpass( agent , &inter , &oppcycle))
			continue;
					if ( (*it)->posCount() > 3)
			continue;
			

	

					Passpoint mp;

		mp.point= (*it)->pos(); 

			mp.number=(*it)->unum();

				mp.value=0.0;

					mPasspoints.push_back(mp);
		}

		if( mPasspoints.size() < 1)
		{
			//std::cout<<"no pass found"<<std::endl;
			
			return false;
		}



			for(size_t i=0;i<mPasspoints.size();i++)
		{				
				Circle2D cir(mPasspoints[i].point,6.0);			
			mPasspoints[i].value += 10.0*(6.0 - wm.countOpponentsIn(cir,10.0,true));							
			mPasspoints[i].value += 10*(mPasspoints[i].point.x);			
			//mPasspoints[i].value -= 5* fabs(wm.self().pos().y - mPasspoints[i].point.y);
				if(mPasspoints[i].point.x > 35.0 && mPasspoints[i].point.absY() < 14.0)			
			mPasspoints[i].value += 500.0;
		}
			
			double max = -10000;
			
				for(size_t i=0;i < mPasspoints.size();i++)
		{
		if (mPasspoints[i].value > max)
		{
		 max = mPasspoints[i].value;
				mpasspoint=mPasspoints[i].point;	
					mUnum = mPasspoints[i].number;
		}

							*pass = mpasspoint;
					

		}
return true;
		}
		 
		/*can pass or can not pass*/
		/*..........................................................*/
		bool Pass::canpass(PlayerAgent * agent ,Vector2D *inter , int * oppcycle)
		{	
			
		const rcsc::WorldModel & wm = agent->world();
		const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
		const PlayerPtrCont::const_iterator opps_end = 	opps.end();
						
						for ( PlayerPtrCont::const_iterator that = opps.begin();
						  that != opps_end;
						  ++that )   
						{
							if ((*that)->posCount() > 2)
							continue;
							rcsc::Line2D line(wm.ball().pos() , *inter);
							rcsc::Line2D prend = line.perpendicular((*that)->pos());
							
							const rcsc::Vector2D target_point = prend.intersection(line);
							const Vector2D ballpos = wm.ball().pos();
							const Vector2D ballvel = wm.ball().vel();
							* oppcycle = Bhv_Predictor::predict_player_reach_cycle ( (*that) , target_point , 1.0,1.0,3,1,1,false);
							 
							double end_speed = 2.2;
							double first_speed = 20.0;
							do
							{
								first_speed
									= calc_first_term_geom_series_last
									( end_speed,
									  (*inter).dist(wm.ball().pos()),
									  ServerParam::i().ballDecay() );
								if ( first_speed < ServerParam::i().ballSpeedMax() )
								{
									break;
								}
								end_speed -= 0.07;
							}
							while ( end_speed > 0.4 );
							
							int ballcycle = (int) rcsc::calc_length_geom_series( first_speed,
										   wm.ball().pos().dist( target_point ),
										   rcsc::ServerParam::i().ballDecay() );
							
							if(*oppcycle<ballcycle+3)	
					
								{
									return false;
								}
						}
				return true;
		}
