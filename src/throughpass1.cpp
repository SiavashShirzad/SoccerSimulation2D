                                   //siavash
/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "bhv_predictor.h"
#include "throughpass1.h"

#include "body_kick_to_corner.h"
#include <rcsc/player/say_message_builder.h>

#include <rcsc/action/shoot_table2008.h>
#include <rcsc/action/body_advance_ball.h>
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
#include <rcsc/geom/line_2d.h>
#include <rcsc/geom/triangle_2d.h>
#include <rcsc/action/body_kick_one_step.h>
#include <rcsc/geom/circle_2d.h>
using namespace std;
using namespace rcsc;	
/*-------------------------------------------------------------------*/
/*!

*/
bool
ThroughPass1::execute( rcsc::PlayerAgent * agent )
{ 

const rcsc::WorldModel & wm = agent->world();

Vector2D mpasspoint;

	if( get_Passpoint( agent, &mpasspoint ) == false )
		return false;

int kick_step = ( agent->world().gameMode().type() != GameMode::PlayOn
                      && agent->world().gameMode().type() != GameMode::GoalKick_
                      ? 1
                      : 3 );
	double end_speed = 0.75;
    double first_speed = 8.0;
    do
    {
        first_speed
            = calc_first_term_geom_series_last
            ( end_speed,
              mpasspoint.dist(wm.ball().pos()),
              ServerParam::i().ballDecay() );
        //= required_first_speed( receiver_dist,
        //ServerParam::i().ballDecay(),
        //end_speed );
        if ( first_speed < ServerParam::i().ballSpeedMax() )
        {
            break;
        }
        end_speed -= 0.01;
    }
    while ( end_speed > 0.5 );

    if ( ! Body_SmartKick( mpasspoint,
                           first_speed,
							first_speed * 0.96,
                           kick_step ).execute( agent ) )
    {
        	return false;
    }


    if ( agent->config().useCommunication() )
    {
		Vector2D target_buf = mpasspoint - agent->world().ball().pos();
        target_buf.setLength( 1.0 );

        agent->addSayMessage( new PassMessage(mUnum,
                                            
                                               mpasspoint + target_buf,
											
											agent->effector().queuedNextBallPos(),
    
                                                agent->effector().queuedNextBallVel() ) );
    
    std::cout<<"through pass1"<<std::endl;
    return true;
    
    }
	

    if ( agent->config().useCommunication() )
    {
		Vector2D target_buf = mpasspoint - agent->world().ball().pos();
        target_buf.setLength( 1.0 );

        agent->addSayMessage( new PassMessage(mUnum,
                                            
                                               mpasspoint + target_buf,
											
											agent->effector().queuedNextBallPos(),
    
                                                agent->effector().queuedNextBallVel() ) );
    
    std::cout<<"through pass1"<<std::endl;
    return true;
    
    }
    return false;


}


bool ThroughPass1::get_Passpoint(PlayerAgent * agent,Vector2D * pass)

{

const rcsc::WorldModel & wm = agent->world();
	Circle2D cpass(wm.self().pos(),5.0);
	
	//if (! wm.existOpponentIn(cpass,10.0,true))
	//	return false;
	

// loop candidate teammates

	
//if (wm.self().pos().x > 36.0)
//continue;
vector<pair<Vector2D,double> > passpoints;
vector<pair<int,double> > unums;
const rcsc::PlayerPtrCont & opps = wm.teammatesFromSelf();
const PlayerPtrCont::const_iterator opps_end = 	opps.end();
    const PlayerPtrCont::const_iterator
        t_end = wm.teammatesFromSelf().end();
    for ( PlayerPtrCont::const_iterator
              it = wm.teammatesFromSelf().begin();
          it != t_end;
          ++it)
        
    {
    if ((*it)->unum()==11)
      continue;
		if ((*it)->goalie() )
			continue;
		if ((*it)->posCount() > 3)
			continue;
		if ((*it)->pos().x<wm.self().pos().x)
			continue;
		Vector2D tmpos = (*it)->pos();
	for(double d=5;d>=-5;d-=5)
		{
			for(int i=0;i<=25;i=i+1)
			{
				Vector2D endpos = tmpos + Vector2D::polar2vector(i,d);
				Line2D l(tmpos,endpos);
				Line2D b(wm.ball().pos(),endpos);
				Vector2D inter = l.intersection(b);
				
				if ( inter.x > 48.0 || inter.absY() >30.0 )
					continue;
				if (inter.x < wm.offsideLineX())
					continue;
				int oppcycle; 
				int oppcycle1;
				if(!canpass( agent , &inter , &oppcycle, &oppcycle1))
					continue;
				if ((*it)->pos().dist(wm.self().pos())>25)
          continue ;
				
				rcsc::Line2D line(wm.ball().pos() , inter);
				rcsc::Line2D prend = line.perpendicular((*it)->pos());
				
				const rcsc::Vector2D target_point = prend.intersection(line);
				const Vector2D ballpos = wm.ball().pos();
				const Vector2D ballvel = wm.ball().vel();
				int tmmcycle = (*it)->playerTypePtr()->cyclesToReachDistance( (*it)->pos().dist(inter));
                     
				
				if(oppcycle1-3<tmmcycle+4.0)
				continue;
				passpoints.push_back(make_pair(inter,0.0));
				unums.push_back( make_pair((*it)->unum() , 0.0) );
			}
			
		}
	}
	if(passpoints.size() < 1)
		return false;
		
	for(int i=0;i<passpoints.size();i++)
		{
		Circle2D cir(passpoints[i].first,15.0);
		passpoints[i].second += 10.0*(10.0 - wm.countOpponentsIn(cir,10.0,true));
		unums[i].second += 10.0*(10.0 - wm.countOpponentsIn(cir,10.0,true));
		Sector2D trans(wm.ball().pos(),0.0,wm.ball().pos().dist(passpoints[i].first),(passpoints[i].first - wm.ball().pos()).th() - 45.0,( passpoints[i].first-wm.ball().pos()).th() + 45.0);
		passpoints[i].second += 10.0*(10.0 - wm.countOpponentsIn(trans,10.0,true));
		unums[i].second += 10.0*(10.0 - wm.countOpponentsIn(trans,10.0,true));		
		//if (passpoints[i].first.x > wm.self().pos().x)
		//passpoints[i].second += 6*fabs(wm.self().pos().x - passpoints[i].first.x);
		//unums[i].second += 6*fabs(wm.self().pos().x - passpoints[i].first.x);
		if (passpoints[i].first.absY() > wm.self().pos().absY())
		passpoints[i].second += 10* fabs(passpoints[i].first.y - wm.self().pos().y );
		unums[i].second += 10* fabs(passpoints[i].first.y - wm.self().pos().y );
		}
		double max = -10000;
	
		for(int i=0;i < passpoints.size();i++)
		{
			if (passpoints[i].second > max)
			{
				max = passpoints[i].second;
				*pass=passpoints[i].first;			
			}
		}
		for(int i=0;i < unums.size();i++)
		{
			if (unums[i].second == max)   
			{
				mUnum = unums[i].first;
				break;			
			}
		}
		
		return true;
}

bool ThroughPass1::canpass(PlayerAgent * agent ,Vector2D *inter , int * oppcycle , int * oppcycle1)
{	
	
const rcsc::WorldModel & wm = agent->world();
const rcsc::PlayerPtrCont & opps = wm.opponentsFromSelf();
const PlayerPtrCont::const_iterator opps_end = 	opps.end();
				
                for ( PlayerPtrCont::const_iterator that = opps.begin();
                  that != opps_end;
                  ++that )   
				{
					if ((*that)->posCount() > 3)   
					continue;
					rcsc::Line2D line(wm.ball().pos() , *inter);
					rcsc::Line2D prend = line.perpendicular((*that)->pos());
					
					const rcsc::Vector2D target_point = prend.intersection(line);
					const Vector2D ballpos = wm.ball().pos();
					const Vector2D ballvel = wm.ball().vel();
		    * oppcycle = Bhv_Predictor::predict_player_reach_cycle ( (*that) , target_point , 1.0,1.0,3,1,1,false);
                    * oppcycle1 =Bhv_Predictor::predict_player_reach_cycle ( (*that) , *inter , 1.0,1.0,3,1,1,false);
                     
					double end_speed = 0.70;
				    double first_speed = 8.0;
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
					Sector2D trans(wm.ball().pos(),0.0,wm.ball().pos().dist(*inter),(*inter - wm.ball().pos()).th() - 25.0,( *inter-wm.ball().pos()).th() + 25.0);
					if(*oppcycle-2.0<ballcycle+2.0|| wm.existOpponentIn(trans,4,true) )	
			
						{
							return false;
						}
				}
		return true;
}
