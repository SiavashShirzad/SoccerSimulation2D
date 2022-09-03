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

#include "sample_player.h"

#include "strategy.h"
#include "soccer_role.h"

#include "bhv_goalie_free_kick.h"
#include "bhv_penalty_kick.h"
#include "bhv_pre_process.h"
#include "bhv_set_play.h"
#include "bhv_set_play_kick_in.h"
#include "bhv_set_play_indirect_free_kick.h"

#include <rcsc/formation/formation.h>
#include <rcsc/action/kick_table.h>
#include <rcsc/player/intercept_table.h>
#include <rcsc/player/say_message_builder.h>
#include <rcsc/player/audio_sensor.h>
#include <rcsc/player/freeform_parser.h>

#include <rcsc/common/basic_client.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/player_param.h>
#include <rcsc/common/audio_memory.h>
#include <rcsc/common/say_message_parser.h>
#include <rcsc/common/free_message_parser.h>

#include <rcsc/param/param_map.h>
#include <rcsc/param/cmd_line_parser.h>

#include <boost/shared_ptr.hpp>

#include <sstream>

#include "tornado_logger.h"
/*-------------------------------------------------------------------*/



/*!

*/
SamplePlayer::SamplePlayer()
    : PlayerAgent()
{
    typedef boost::shared_ptr< rcsc::SayMessageParser > SMP;

    boost::shared_ptr< rcsc::AudioMemory > audio_memory( new rcsc::AudioMemory );

    M_worldmodel.setAudioMemory( audio_memory );

    addSayMessageParser( SMP( new rcsc::BallMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::PassMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::InterceptMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::GoalieMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::OffsideLineMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::DefenseLineMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::WaitRequestMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::PassRequestMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::DribbleMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::BallGoalieMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::OnePlayerMessageParser( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::BallPlayerMessageParser( audio_memory ) ) );

    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 9 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 8 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 7 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 6 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 5 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 4 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 3 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 2 >( audio_memory ) ) );
    addSayMessageParser( SMP( new rcsc::FreeMessageParser< 1 >( audio_memory ) ) );

    typedef boost::shared_ptr< rcsc::FreeformParser > FP;

    setFreeformParser( FP( new rcsc::FreeformParser( M_worldmodel ) ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
SamplePlayer::~SamplePlayer()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
bool
SamplePlayer::initImpl( rcsc::CmdLineParser & cmd_parser )
{
    bool result = rcsc::PlayerAgent::initImpl( cmd_parser );

    // read additional options
    result &= Strategy::instance().init( cmd_parser );

    rcsc::ParamMap my_params;
#if 0
    std::string formation_conf;
    my_params.add()
        ( &formation_conf, "fconf" )
        ;

    result &= cmd_parser.parse( my_params );
#endif

    if ( cmd_parser.count( "help" ) > 0 )
    {
        my_params.printHelp( std::cout );
        return false;
    }

    if ( ! result )
    {
        return false;
    }

    if ( cmd_parser.failed() )
    {
        std::cerr << "player: ***WARNING*** detected unsupported options: ";
        cmd_parser.print( std::cerr );
        std::cerr << std::endl;
    }

    if ( ! Strategy::instance().read( config().configDir() ) )
    {
        std::cerr << "***ERROR*** Failed to read team strategy." << std::endl;
        return false;
    }

    //////////////////////////////////////////////////////////////////
    // Add your code here.
    //////////////////////////////////////////////////////////////////


    return true;
}

/*-------------------------------------------------------------------*/
/*!
  main decision
  virtual method in super class
*/
void
SamplePlayer::actionImpl()
{
    if ( audioSensor().trainerMessageTime() == world().time() )
    {
        std::cerr << world().teamName() << ' '
                  << world().self().unum() << ": "
                  << world().time()
                  << " hear trainer message ["
                  << audioSensor().trainerMessage()
                  << "]"
                  << std::endl;
    }

    if ( audioSensor().freeformMessageTime() == world().time() )
    {
        std::cerr << world().teamName() << ' '
                  << world().self().unum() << ": "
                  << world().time()
                  << " hear freeform message ["
                  << audioSensor().freeformMessage()
                  << "]"
                  << std::endl;
    }

    //
    // update strategy (formation, ...)
    //

    Strategy::instance().update( world() );

    //////////////////////////////////////////////////////////////
    // check tackle expires
    // check self position accuracy
    // ball search
    // check queued intention
    // check simultaneous kick
    if ( Bhv_PreProcess().execute( this ) )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            "%s:%d: preprocess done"
                            ,__FILE__, __LINE__ );
        return;
    }

    //////////////////////////////////////////////////////////////
    // create current role
    SoccerRole::Ptr role_ptr = Strategy::i().createRole( world().self().unum(), world() );

    if ( ! role_ptr )
    {
        std::cerr << config().teamName() << ": "
                  << world().self().unum()
                  << " Error. Role is not registerd.\nExit ..."
                  << std::endl;
        M_client->setServerAlive( false );
        return;
    }

    //////////////////////////////////////////////////////////////
    // play_on mode
    if ( world().gameMode().type() == rcsc::GameMode::PlayOn  )
    {
        role_ptr->execute( this );
        return;
    }

    rcsc::Vector2D home_pos = Strategy::i().getPosition( world().self().unum() );
    if ( ! home_pos.valid() )
    {
        std::cerr << config().teamName() << ": "
                  << world().self().unum()
                  << " ***ERROR*** illegal home position."
                  << std::endl;
        home_pos.assign( 0.0, 0.0 );
    }

    debugClient().addLine( rcsc::Vector2D( home_pos.x, home_pos.y - 0.25 ),
                           rcsc::Vector2D( home_pos.x, home_pos.y + 0.25 ) );
    debugClient().addLine( rcsc::Vector2D( home_pos.x - 0.25, home_pos.y ),
                           rcsc::Vector2D( home_pos.x + 0.25, home_pos.y ) );
    debugClient().addCircle( home_pos, 0.5 );

    //////////////////////////////////////////////////////////////
    // kick_in or corner_kick
    if ( ( world().gameMode().type() == rcsc::GameMode::KickIn_
           || world().gameMode().type() == rcsc::GameMode::CornerKick_ )
         && world().ourSide() == world().gameMode().side() )
    {
        if ( world().self().goalie() )
        {
            Bhv_GoalieFreeKick().execute( this );
        }
        else
        {
            Bhv_SetPlayKickIn( home_pos ).execute( this );
        }
        return;
    }

    //////////////////////////////////////////////////////////////
    if ( world().gameMode().type() == rcsc::GameMode::BackPass_
         || world().gameMode().type() == rcsc::GameMode::IndFreeKick_ )
    {
        Bhv_SetPlayIndirectFreeKick( home_pos ).execute( this );
        return;
    }

    //////////////////////////////////////////////////////////////
    // penalty kick mode
    if ( world().gameMode().isPenaltyKickMode() )
    {
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": penalty kick" );
        Bhv_PenaltyKick().execute( this );
        return;
    }

    //////////////////////////////////////////////////////////////
    // goalie free kick mode
    if ( world().self().goalie() )
    {
        Bhv_GoalieFreeKick().execute( this );
        return;
    }

    //////////////////////////////////////////////////////////////
    // other set play mode

    Bhv_SetPlay( home_pos ).execute( this );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
SamplePlayer::handleServerParam()
{
    if ( rcsc::KickTable::instance().createTables() )
    {
        std::cerr << world().teamName() << ' '
                  << world().self().unum() << ": "
                  << " KickTable created."
                  << std::endl;
    }
    else
    {
        std::cerr << world().teamName() << ' '
                  << world().self().unum() << ": "
                  << " KickTable failed..."
                  << std::endl;
        M_client->setServerAlive( false );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
SamplePlayer::handlePlayerParam()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
void
SamplePlayer::handlePlayerType()
{

}

/*-------------------------------------------------------------------*/
/*!
  communication decision.
  virtual method in super class
*/
void
SamplePlayer::communicationImpl()
{
    if ( ! config().useCommunication()
         || world().gameMode().type() == rcsc::GameMode::BeforeKickOff
         || world().gameMode().type() == rcsc::GameMode::AfterGoal_
         || world().gameMode().isPenaltyKickMode() )
    {
        return;
    }

    sayBall();
    sayDefenseLine();
    sayGoalie();
    sayIntercept();
    sayOffsideLine();
    saySelf();

#if 0
    const int len = effector().getSayMessageLength();
    switch ( len ) {
    case 7:
        addSayMessage( new rcsc::FreeMessage< 2 >( "10" ) );
        break;
    case 6:
        addSayMessage( new rcsc::FreeMessage< 3 >( "210" ) );
        break;
    case 5:
        addSayMessage( new rcsc::FreeMessage< 4 >( "3210" ) );
        break;
    case 4:
        addSayMessage( new rcsc::FreeMessage< 5 >( "43210" ) );
        break;
    case 3:
        addSayMessage( new rcsc::FreeMessage< 6 >( "543210" ) );
        break;
    case 2:
        addSayMessage( new rcsc::FreeMessage< 7 >( "6543210" ) );
        break;
    case 1:
        addSayMessage( new rcsc::FreeMessage< 8 >( "76543210" ) );
        break;
    default:
        break;
    }
#endif

    attentiontoSomeone();
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
SamplePlayer::sayBall()
{
    // ball Info: seen at current
    const int len = effector().getSayMessageLength();
    if ( static_cast< int >( len + rcsc::BallMessage::slength() )
         > rcsc::ServerParam::i().playerSayMsgSize()
         || world().ball().posCount() > 0
         || world().ball().velCount() > 0 )
    {
        return false;
    }

    const rcsc::PlayerObject * ball_nearest_teammate = NULL;
    const rcsc::PlayerObject * second_ball_nearest_teammate = NULL;

    const rcsc::PlayerPtrCont::const_iterator t_end = world().teammatesFromBall().end();
    for ( rcsc::PlayerPtrCont::const_iterator t = world().teammatesFromBall().begin();
          t != t_end;
          ++t )
    {
        if ( (*t)->isGhost() || (*t)->posCount() >= 10 ) continue;

        if ( ! ball_nearest_teammate )
        {
            ball_nearest_teammate = *t;
        }
        else if ( ! second_ball_nearest_teammate )
        {
            second_ball_nearest_teammate = *t;
            break;
        }
    }

    bool send_ball = false;
    if ( world().self().isKickable()  // I am kickable
         || ( ! ball_nearest_teammate
              || ( ball_nearest_teammate->distFromBall()
                   > world().ball().distFromSelf() - 3.0 ) ) // I am nearest to ball
         || ( second_ball_nearest_teammate
              && ( ball_nearest_teammate->distFromBall()  // nearest to ball teammate is
                   > rcsc::ServerParam::i().visibleDistance() - 0.5 ) // over vis dist
              && ( second_ball_nearest_teammate->distFromBall()
                   > world().ball().distFromSelf() - 5.0 ) ) ) // I am second
    {
        send_ball = true;
    }

    // send ball & goalie
    if ( send_ball
         && static_cast< int >( len + rcsc::BallGoalieMessage::slength() )
         <= rcsc::ServerParam::i().playerSayMsgSize()
         && world().ball().pos().x > 34.0
         && world().ball().pos().absY() < 20.0 )
    {
        const rcsc::PlayerObject * opp_goalie = world().getOpponentGoalie();
        if ( opp_goalie
             && opp_goalie->posCount() == 0
             && opp_goalie->bodyCount() == 0
             && opp_goalie->unum() != rcsc::Unum_Unknown
             && opp_goalie->distFromSelf() < 25.0
             && opp_goalie->pos().x > 52.5 - 16.0
             && opp_goalie->pos().x < 52.5
             && opp_goalie->pos().absY() < 20.0 )
        {
            addSayMessage( new rcsc::BallGoalieMessage( effector().queuedNextBallPos(),
                                                        effector().queuedNextBallVel(),
                                                        opp_goalie->pos(),
                                                        opp_goalie->body() ) );
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": say ball/goalie status" );
            debugClient().addMessage( "SayG" );
            return true;
        }
    }

    // send only ball
    if ( send_ball )
    {
        addSayMessage( new rcsc::BallMessage( effector().queuedNextBallPos(),
                                              effector().queuedNextBallVel() ) );
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": say ball status" );
        debugClient().addMessage( "Sayb" );
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
SamplePlayer::sayGoalie()
{
    // goalie info: ball is in chance area
    int len = effector().getSayMessageLength();
    if ( static_cast< int >( len + rcsc::GoalieMessage::slength() )
         > rcsc::ServerParam::i().playerSayMsgSize()
         || world().ball().pos().x < 34.0
         || world().ball().pos().absY() > 20.0 )
    {
        return false;
    }

    const rcsc::PlayerObject * opp_goalie = world().getOpponentGoalie();
    if ( opp_goalie
         && opp_goalie->posCount() == 0
         && opp_goalie->bodyCount() == 0
         && opp_goalie->unum() != rcsc::Unum_Unknown
         && opp_goalie->distFromSelf() < 25.0 )
    {
        addSayMessage( new rcsc::GoalieMessage( opp_goalie->unum(),
                                                opp_goalie->pos(),
                                                opp_goalie->body() ) );
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": say goalie info: %d pos=(%.1f %.1f) body=%.1f",
                            opp_goalie->unum(),
                            opp_goalie->pos().x,
                            opp_goalie->pos().y,
                            opp_goalie->body().degree() );
        debugClient().addMessage( "Sayg" );
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
SamplePlayer::sayIntercept()
{
    // intercept info
    int len = effector().getSayMessageLength();
    if ( static_cast< int >( len + rcsc::InterceptMessage::slength() )
         > rcsc::ServerParam::i().playerSayMsgSize()
         || world().ball().posCount() > 0
         || world().ball().velCount() > 0 )
    {
        return false;
    }

    int self_min = world().interceptTable()->selfReachCycle();
    int mate_min = world().interceptTable()->teammateReachCycle();
    int opp_min = world().interceptTable()->opponentReachCycle();

    if ( world().self().isKickable() )
    {
        double next_dist =  effector().queuedNextMyPos().dist( effector().queuedNextBallPos() );
        if ( next_dist > world().self().kickableArea() )
        {
            self_min = 10000;
        }
        else
        {
            self_min = 0;
        }
    }

    if ( self_min <= mate_min
         && self_min <= opp_min
         && self_min <= 10 )
    {
        addSayMessage( new rcsc::InterceptMessage( true,
                                                   world().self().unum(),
                                                   self_min ) );
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": say self intercept info %d",
                            self_min );
        debugClient().addMessage( "Sayi_self" );
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
SamplePlayer::sayOffsideLine()
{
    int len = effector().getSayMessageLength();
    if ( static_cast< int >( len + rcsc::OffsideLineMessage::slength() )
         > rcsc::ServerParam::i().playerSayMsgSize() )
    {
        return false;
    }

    if ( world().offsideLineCount() <= 1
         && world().opponentsFromSelf().size() >= 8
         && 0.0 < world().ball().pos().x
         && world().ball().pos().x < 37.0
         && world().ball().pos().x > world().offsideLineX() - 20.0
         && std::fabs( world().self().pos().x - world().offsideLineX() ) < 20.0
         )
    {
        addSayMessage( new rcsc::OffsideLineMessage( world().offsideLineX() ) );
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": say offside line %.1f",
                            world().offsideLineX() );
        debugClient().addMessage( "Sayo" );
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
SamplePlayer::sayDefenseLine()
{
    int len = effector().getSayMessageLength();
    if ( static_cast< int >( len + rcsc::DefenseLineMessage::slength() )
         > rcsc::ServerParam::i().playerSayMsgSize()
         || std::fabs( world().self().pos().x - world().defenseLineX() ) > 40.0
         )
    {
        return false;
    }

    int opp_min = world().interceptTable()->opponentReachCycle();

    rcsc::Vector2D opp_trap_pos = world().ball().inertiaPoint( opp_min );

    if ( world().self().goalie()
         && -40.0 < opp_trap_pos.x
         && opp_trap_pos.x > 10.0 )
    {
        addSayMessage( new rcsc::DefenseLineMessage( world().defenseLineX() ) );
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": say defense line %.1f",
                            world().defenseLineX() );
        debugClient().addMessage( "Sayd" );
        return true;
    }

    return false;
}


/*-------------------------------------------------------------------*/
/*!

*/
bool
SamplePlayer::saySelf()
{
    const int len = effector().getSayMessageLength();
    if ( static_cast< int >( len + rcsc::DefenseLineMessage::slength() )
         > rcsc::ServerParam::i().playerSayMsgSize()
         || std::fabs( world().self().pos().x - world().defenseLineX() ) > 40.0
         )
    {
        return false;
    }

    bool send_self = false;

    if ( len > 0 )
    {
        send_self = true;
    }

    if ( ! send_self )
    {
        const rcsc::PlayerObject * ball_nearest_teammate = NULL;
        const rcsc::PlayerObject * second_ball_nearest_teammate = NULL;

        const rcsc::PlayerPtrCont::const_iterator t_end = world().teammatesFromBall().end();
        for ( rcsc::PlayerPtrCont::const_iterator t = world().teammatesFromBall().begin();
              t != t_end;
              ++t )
        {
            if ( (*t)->isGhost() || (*t)->posCount() >= 10 ) continue;

            if ( ! ball_nearest_teammate )
            {
                ball_nearest_teammate = *t;
            }
            else if ( ! second_ball_nearest_teammate )
            {
                second_ball_nearest_teammate = *t;
                break;
            }
        }

        if ( ball_nearest_teammate
             && ball_nearest_teammate->distFromBall() < world().ball().distFromSelf()
             && ( ! second_ball_nearest_teammate
                  || second_ball_nearest_teammate->distFromBall() > world().ball().distFromSelf() )
             )
        {
            send_self = true;
        }
    }

    if ( send_self )
    {
        addSayMessage( new rcsc::OnePlayerMessage( world().self().unum(),
                                                   effector().queuedNextMyPos() ) );
        rcsc::dlog.addText( rcsc::Logger::TEAM,
                            __FILE__": say self status" );
        debugClient().addMessage( "Say1_self" );
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
SamplePlayer::attentiontoSomeone()
{
    if ( world().self().pos().x > world().offsideLineX() - 15.0
         && world().interceptTable()->selfReachCycle() <= 3 )
    {
        std::vector< const rcsc::PlayerObject * > candidates;
        const rcsc::PlayerPtrCont::const_iterator end = world().teammatesFromSelf().end();
        for ( rcsc::PlayerPtrCont::const_iterator p = world().teammatesFromSelf().begin();
              p != end;
              ++p )
        {
            if ( (*p)->goalie() ) continue;
            if ( (*p)->unum() == rcsc::Unum_Unknown ) continue;
            if ( (*p)->pos().x > world().offsideLineX() + 1.0 ) continue;

            if ( (*p)->distFromSelf() > 20.0 ) break;

            candidates.push_back( *p );
        }

        const rcsc::PlayerObject * target_teammate = NULL;
        double max_x = -100.0;
        for ( std::vector< const rcsc::PlayerObject * >::const_iterator p = candidates.begin();
              p != candidates.end();
              ++p )
        {
            if ( (*p)->pos().x > max_x )
            {
                max_x = (*p)->pos().x;
                target_teammate = *p;
            }
        }

        if ( target_teammate )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": attentionto most front teammate",
                                world().offsideLineX() );
            debugClient().addMessage( "AttFrontMate" );
            doAttentionto( world().ourSide(), target_teammate->unum() );
            return;
        }

        // maybe ball owner
        if ( world().self().attentiontoUnum() > 0 )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": attentionto off. maybe ball owner",
                                world().offsideLineX() );
            debugClient().addMessage( "AttOffBOwner" );
            doAttentiontoOff();
        }
        return;
    }

    const rcsc::PlayerObject * fastest_teammate
        = world().interceptTable()->fastestTeammate();

    int self_min = world().interceptTable()->selfReachCycle();
    int mate_min = world().interceptTable()->teammateReachCycle();
    int opp_min = world().interceptTable()->opponentReachCycle();

    if ( ! fastest_teammate )
    {
        if ( world().self().attentiontoUnum() > 0 )
        {
            rcsc::dlog.addText( rcsc::Logger::TEAM,
                                __FILE__": attentionto off. no fastest teammate",
                                world().offsideLineX() );
            debugClient().addMessage( "AttOffNoMate" );
            doAttentiontoOff();
        }
        return;
    }

    if ( mate_min < self_min
         && mate_min <= opp_min
         && fastest_teammate->unum() != rcsc::Unum_Unknown )
    {
        // set attention to ball nearest teammate
        debugClient().addMessage( "AttBallOwner" );
        doAttentionto( world().ourSide(), fastest_teammate->unum() );
        return;
    }

    debugClient().addMessage( "AttOff" );
    doAttentiontoOff();
}
