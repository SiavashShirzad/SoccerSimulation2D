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
#include "config.h"
#endif

#include "sample_coach.h"

#include <rcsc/coach/coach_command.h>
#include <rcsc/coach/coach_config.h>
#include <rcsc/coach/global_visual_sensor.h>

#include <rcsc/common/basic_client.h>
#include <rcsc/common/player_param.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/player_type.h>
#include <rcsc/common/audio_memory.h>
#include <rcsc/common/say_message_parser.h>
#include <rcsc/common/free_message_parser.h>

#include <rcsc/param/param_map.h>
#include <rcsc/param/cmd_line_parser.h>

#include <functional>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iostream>
#include <cstdio>

#include "team_logo.xpm"

/////////////////////////////////////////////////////////

struct IsIgnoredStaminaIncMax
    : public std::unary_function< const rcsc::PlayerType *, bool > {
    const double M_ignore_thr;

    IsIgnoredStaminaIncMax( const double & thr )
        : M_ignore_thr( thr )
      { }

    result_type operator()( argument_type arg ) const
      {
          return arg->staminaIncMax() < M_ignore_thr;
      }
};


/////////////////////////////////////////////////////////

struct IsIgnoredSpeedMax
    : public std::unary_function< const rcsc::PlayerType *, bool > {
    const double M_ignore_thr;

    IsIgnoredSpeedMax( const double & thr )
        : M_ignore_thr( thr )
      { }

    result_type operator()( argument_type arg ) const
      {
          return arg->realSpeedMax() < M_ignore_thr;
      }
};


/////////////////////////////////////////////////////////

struct TypeStaminaIncComp
    : public std::binary_function< const rcsc::PlayerType *,
                                   const rcsc::PlayerType *,
                                   bool > {

    result_type operator()( first_argument_type lhs,
                            second_argument_type rhs ) const
      {
          return lhs->staminaIncMax() > rhs->staminaIncMax();
      }

};

/////////////////////////////////////////////////////////

struct RealSpeedMaxCmp
    : public std::binary_function< const rcsc::PlayerType *,
                                   const rcsc::PlayerType *,
                                   bool > {

    result_type operator()( first_argument_type lhs,
                            second_argument_type rhs ) const
      {
          return lhs->realSpeedMax() < rhs->realSpeedMax();
      }

};

/////////////////////////////////////////////////////////

struct MaxSpeedReachStepCmp
    : public std::binary_function< const rcsc::PlayerType *,
                                   const rcsc::PlayerType *,
                                   bool > {

    result_type operator()( first_argument_type lhs,
                            second_argument_type rhs ) const
      {
          return lhs->cyclesToReachMaxSpeed() > rhs->cyclesToReachMaxSpeed();
      }

};

/*-------------------------------------------------------------------*/
/*!

 */
SampleCoach::SampleCoach()
    : CoachAgent()
{
    //
    // register audio memory & say message parsers
    //
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

    //
    //
    //

    for ( int i = 0; i < 11; ++i )
    {
        // init map values
        M_player_type_id[i] = rcsc::Hetero_Default;
    }

    for ( int i = 0; i < 11; ++i )
    {
        M_opponent_player_types[i] = rcsc::Hetero_Default;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
SampleCoach::~SampleCoach()
{

}

/*-------------------------------------------------------------------*/
/*!

 */
bool
SampleCoach::initImpl( rcsc::CmdLineParser & cmd_parser )
{
    bool result = rcsc::CoachAgent::initImpl( cmd_parser );

    rcsc::ParamMap my_params;
#if 0
    std::string formation_conf;
    int role_number = 0;
    bool use_clang = false;
    my_map.add()
        ( "fconf", "", &formation_conf, "set the formation configuration file." )
        ( "strategy-number", "", &strategy_number, "set the strategy number." )
        ( "use_clang", "", rcsc::BoolSwitch( &use_clang ), "" )
        ;
#endif
    cmd_parser.parse( my_params );

    if ( cmd_parser.count( "help" ) )
    {
        my_params.printHelp( std::cout );
        return false;
    }

    if ( cmd_parser.failed() )
    {
        std::cerr << "coach: ***WARNING*** detected unsupported options: ";
        cmd_parser.print( std::cerr );
        std::cerr << std::endl;
    }

    if ( ! result )
    {
        return false;
    }

    if ( config().useTeamGraphic() )
    {
        if ( config().teamGraphicFile().empty() )
        {
            M_team_graphic.createXpmTiles( team_logo_xpm );
        }
        else
        {
            M_team_graphic.readXpmFile( config().teamGraphicFile().c_str() );
        }
    }

    //////////////////////////////////////////////////////////////////
    // Add your code here.
    //////////////////////////////////////////////////////////////////


    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SampleCoach::actionImpl()
{
    if ( world().time().cycle() == 0
         && config().useTeamGraphic()
         && M_team_graphic.tiles().size() != teamGraphicOKSet().size() )
    {
        std::cerr << world().time() << "sendTeamGraphic " << std::endl;
        sendTeamGraphic();
    }

    doSubstitute();
    sayPlayerTypes();

//     if ( world().canSendFreeform() )
//     {
//         char msg[128];
//         std::sprintf( msg, "freeform message %d/%d",
//                       world().freeformSendCount(),
//                       world().freeformAllowedCount() );
//         doSayFreeform( msg );
//     }

//     if ( world().time().cycle() > 0 )
//     {
//         M_client->setServerAlive( false );
//     }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SampleCoach::doSubstitute()
{
    static bool S_first_substituted = false;

    if ( config().useHetero()
         && ! S_first_substituted
         && world().time().cycle() == 0
         && world().time().stopped() > 10 )
    {
        doFirstSubstitute();
        S_first_substituted = true;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SampleCoach::doFirstSubstitute()
{
    std::vector< const rcsc::PlayerType * > candidates;

    std::fprintf( stderr,
                  "id speed step inc  power  stam decay moment dprate   karea  krand effmax effmin\n" );
    for ( rcsc::PlayerTypeSet::PlayerTypeMap::const_iterator it
              = rcsc::PlayerTypeSet::i().playerTypeMap().begin();
          it != rcsc::PlayerTypeSet::i().playerTypeMap().end();
          ++it )
    {
        int pt_max = rcsc::PlayerParam::i().ptMax();
        if ( it->second.id() == rcsc::Hetero_Default )
        {
            if ( rcsc::PlayerParam::i().allowMultDefaultType() )
            {
                pt_max = 11;
            }
            else
            {
                pt_max -= 1; // because goalie is always default type
            }
        }

        for ( int i = 0; i < pt_max; ++i )
        {
            M_available_player_type_id.push_back( it->second.id() );
            candidates.push_back( &(it->second) );
        }

        std::fprintf( stderr,
                      "%d  %.3f  %2d  %.1f %5.1f %5.1f %.3f  %4.1f  %.5f  %.3f  %.2f  %.3f  %.3f\n",
                      it->second.id(),
                      it->second.realSpeedMax(),
                      it->second.cyclesToReachMaxSpeed(),
                      it->second.staminaIncMax(),
                      it->second.getDashPowerToKeepMaxSpeed( rcsc::ServerParam::i() ),
                      it->second.getOneStepStaminaComsumption( rcsc::ServerParam::i() ),
                      it->second.playerDecay(),
                      it->second.inertiaMoment(),
                      it->second.dashPowerRate(),
                      it->second.kickableArea(),
                      it->second.kickRand(),
                      it->second.effortMax(),
                      it->second.effortMin() );
    }

    // uniform number of players that belongs to center defender
    std::vector< int > center_defender_unum;
    // uniform number of players that belongs to side defender
    std::vector< int > side_defender_unum;
    // uniform number of players that belongs to midfielder
    std::vector< int > midfielder_unum;
    // uniform number of players that belongs to forward
    std::vector< int > forward_unum;

    center_defender_unum.push_back( 2 );
    center_defender_unum.push_back( 3 );
    center_defender_unum.push_back( 6 );

    side_defender_unum.push_back( 4 );
    side_defender_unum.push_back( 5 );

    midfielder_unum.push_back( 7 );
    midfielder_unum.push_back( 8 );

    forward_unum.push_back( 9 );
    forward_unum.push_back( 10 );
    forward_unum.push_back( 11 );

    //if ( config().version() < 12.0 )
    if ( rcsc::PlayerParam::i().allowMultDefaultType() )
    {
        for ( std::vector< int >::iterator it = forward_unum.begin();
              it != forward_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            substituteTo( *it, type );
        }

        for ( std::vector< int >::iterator it = center_defender_unum.begin();
              it != center_defender_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            substituteTo( *it, type );
        }

        for ( std::vector< int >::iterator it = side_defender_unum.begin();
              it != side_defender_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            substituteTo( *it, type );
        }

        for ( std::vector< int >::iterator it = midfielder_unum.begin();
              it != midfielder_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            substituteTo( *it, type );
        }
    }
    else
    {
        std::vector< std::pair< int, int > > types;
        types.reserve( 10 );

        for ( std::vector< int >::iterator it = forward_unum.begin();
              it != forward_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            types.push_back( std::make_pair( *it, type ) );
        }

        for ( std::vector< int >::iterator it = center_defender_unum.begin();
              it != center_defender_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            types.push_back( std::make_pair( *it, type ) );
        }

        for ( std::vector< int >::iterator it = side_defender_unum.begin();
              it != side_defender_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            types.push_back( std::make_pair( *it, type ) );
        }

        for ( std::vector< int >::iterator it = midfielder_unum.begin();
              it != midfielder_unum.end();
              ++it )
        {
            int type = getFastestType( candidates );
            if ( type == rcsc::Hetero_Unknown ) continue;
            types.push_back( std::make_pair( *it, type ) );
        }

        substituteTo( types );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SampleCoach::substituteTo( const int unum,
                           const int type )
{
    std::vector< int >::iterator it = std::find( M_available_player_type_id.begin(),
                                                 M_available_player_type_id.end(),
                                                 type );
    if ( it == M_available_player_type_id.end() )
    {
        std::cerr << "***ERROR*** "
                  << config().teamName() << " coach: "
                  << " cannot change the player " << unum
                  << " to type " << type
                  << std::endl;
        return;
    }

    M_available_player_type_id.erase( it );

    std::cout << config().teamName() << " coach: "
              << "change player " << unum
              << " to type " << type
              << std::endl;
    M_player_type_id.insert( std::make_pair( unum, type ) );

    doChangePlayerType( unum, type );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SampleCoach::substituteTo( const std::vector< std::pair< int, int > > & types )
{
    if ( types.empty() )
    {
        std::cerr << "***WARNING*** "
                  << config().teamName() << " coach: "
                  << " empty player type assignment."
                  << std::endl;
        return;
    }

    for ( std::vector< std::pair< int, int > >::const_iterator t = types.begin();
          t != types.end();
          ++t )
    {
        std::vector< int >::iterator it = std::find( M_available_player_type_id.begin(),
                                                     M_available_player_type_id.end(),
                                                     t->second );
        if ( it == M_available_player_type_id.end() )
        {
            std::cerr << "***ERROR*** "
                      << config().teamName() << " coach: "
                      << " cannot change the player " << t->first
                      << " to type " << t->second
                      << std::endl;
            return;
        }
    }

    for ( std::vector< std::pair< int, int > >::const_iterator t = types.begin();
          t != types.end();
          ++t )
    {
        std::vector< int >::iterator it = std::find( M_available_player_type_id.begin(),
                                                     M_available_player_type_id.end(),
                                                     t->second );
        M_available_player_type_id.erase( it );

        std::cout << config().teamName() << " coach: "
                  << "change player " << t->first
                  << " to type " << t->second
                  << std::endl;
        M_player_type_id.insert( *t );
    }

    doChangePlayerTypes( types );
}

/*-------------------------------------------------------------------*/
/*!

 */
int
SampleCoach::getFastestType( PlayerTypePtrCont & candidates )
{
    if ( candidates.empty() )
    {
        return rcsc::Hetero_Unknown;
    }

//     std::cerr << "getFastestType available types = ";
//     for ( PlayerTypePtrCont::iterator it = candidates.begin();
//           it != candidates.end();
//           ++it )
//     {
//         std::cerr << (*it)->id() << ' ';
//     }
//     std::cerr << std::endl;;

    // sort by max speed
    std::sort( candidates.begin(),
               candidates.end(),
               std::not2( RealSpeedMaxCmp() ) );

    PlayerTypePtrCont::iterator best_type = candidates.begin();
    double max_speed = (*best_type)->realSpeedMax();
    int min_cycle = 100;
    for ( PlayerTypePtrCont::iterator it = candidates.begin();
          it != candidates.end();
          ++it )
    {
        if ( (*it)->realSpeedMax() < max_speed - 0.01 )
        {
            break;
        }

        if ( (*it)->cyclesToReachMaxSpeed() < min_cycle )
        {
            best_type = it;
            max_speed = (*best_type)->realSpeedMax();
            min_cycle = (*best_type)->cyclesToReachMaxSpeed();
            continue;
        }

        if ( (*it)->cyclesToReachMaxSpeed() == min_cycle )
        {
            if ( (*it)->getOneStepStaminaComsumption( rcsc::ServerParam::i() )
                 < (*best_type)->getOneStepStaminaComsumption( rcsc::ServerParam::i()) )
            {
                best_type = it;
                max_speed = (*best_type)->realSpeedMax();
            }
        }
    }

    int id = (*best_type)->id();
    candidates.erase( best_type );
    return id;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
SampleCoach::sayPlayerTypes()
{
    if ( ! config().useFreeform() )
    {
        return;
    }

    /*
      format:
      "(player_types (1 0) (2 1) (3 2) (4 3) (5 4) (6 5) (7 6) (8 -1) (9 0) (10 1) (11 2))"
      ->
      (say (freeform "(player_type ...)"))
    */

    static rcsc::GameTime s_last_send_time( 0, 0 );

    if ( ! world().canSendFreeform() )
    {
        return;
    }

    int analyzed_count = 0;

    for ( int unum = 1; unum <= 11; ++unum )
    {
        const int id = world().heteroID( world().theirSide(), unum );

        if ( id != M_opponent_player_types[unum - 1] )
        {
            M_opponent_player_types[unum - 1] = id;

            if ( id != rcsc::Hetero_Unknown )
            {
                ++analyzed_count;
            }
        }
    }

    if ( analyzed_count == 0 )
    {
        return;
    }

    std::string msg;
    msg.reserve( 128 );

    msg = "(player_types ";

    for ( int unum = 1; unum <= 11; ++unum )
    {
        char buf[8];
        snprintf( buf, 8, "(%d %d)",
                  unum, M_opponent_player_types[unum - 1] );
        msg += buf;
    }

    msg += ")";

    doSayFreeform( msg );

    s_last_send_time = world().time();

    std::cout << config().teamName()
              << " coach: "
              << world().time()
              << " send freeform " << msg
              << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
SampleCoach::sendTeamGraphic()
{
    int count = 0;
    for ( rcsc::TeamGraphic::Map::const_reverse_iterator tile
              = M_team_graphic.tiles().rbegin();
          tile != M_team_graphic.tiles().rend();
          ++tile )
    {
        if ( teamGraphicOKSet().find( tile->first ) == teamGraphicOKSet().end() )
        {
            if ( ! doTeamGraphic( tile->first.first,
                                  tile->first.second,
                                  M_team_graphic ) )
            {
                break;
            }
            ++count;
        }
    }

    if ( count > 0 )
    {
        std::cout << config().teamName()
                  << " coach: "
                  << world().time()
                  << " send team_graphic " << count << " tiles"
                  << std::endl;
    }
}
