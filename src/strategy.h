// -*-c++-*-

/*!
  \file strategy.h
  \brief team strategh Header File
*/

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
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

#ifndef AGENT2D_STRATEGY_H
#define AGENT2D_STRATEGY_H

#include "soccer_role.h"

#include <rcsc/formation/formation.h>
#include <rcsc/geom/vector_2d.h>

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <string>

// # define USE_GENERIC_FACTORY 1

namespace rcsc {
class CmdLineParser;
class WorldModel;
}

enum PositionType {
    Position_Left = -1,
    Position_Center = 0,
    Position_Right = 1,
};

class Strategy {
public:
    static const std::string BEFORE_KICK_OFF_CONF;
    static const std::string NORMAL_FORMATION_CONF;
    static const std::string GOALIE_FORMATION_CONF;
    static const std::string GOAL_KICK_OPP_FORMATION_CONF;
    static const std::string GOAL_KICK_OUR_FORMATION_CONF;
    static const std::string GOALIE_CATCH_OPP_FORMATION_CONF;
    static const std::string GOALIE_CATCH_OUR_FORMATION_CONF;
    static const std::string KICKIN_OUR_FORMATION_CONF;
    static const std::string SETPLAY_OPP_FORMATION_CONF;
    static const std::string SETPLAY_OUR_FORMATION_CONF;
    static const std::string INDIRECT_FREEKICK_OPP_FORMATION_CONF;
    static const std::string INDIRECT_FREEKICK_OUR_FORMATION_CONF;

    enum BallArea {
        BA_CrossBlock, BA_DribbleBlock, BA_DribbleAttack, BA_Cross,
        BA_Stopper,    BA_DefMidField,  BA_OffMidField,   BA_ShootChance,
        BA_Danger,

        BA_None
    };

private:
    //
    // factories
    //
#ifndef USE_GENERIC_FACTORY
    typedef std::map< std::string, SoccerRole::Creator > RoleFactory;
    typedef std::map< std::string, rcsc::Formation::Creator > FormationFactory;

    RoleFactory M_role_factory;
    FormationFactory M_formation_factory;
#endif


    //
    // formations
    //

    rcsc::Formation::Ptr M_before_kick_off_formation;
    rcsc::Formation::Ptr M_normal_formation;
    rcsc::Formation::Ptr M_goalie_formation;
    rcsc::Formation::Ptr M_goal_kick_opp_formation;
    rcsc::Formation::Ptr M_goal_kick_our_formation;
    rcsc::Formation::Ptr M_goalie_catch_opp_formation;
    rcsc::Formation::Ptr M_goalie_catch_our_formation;
    rcsc::Formation::Ptr M_kickin_our_formation;
    rcsc::Formation::Ptr M_setplay_opp_formation;
    rcsc::Formation::Ptr M_setplay_our_formation;
    rcsc::Formation::Ptr M_indirect_freekick_opp_formation;
    rcsc::Formation::Ptr M_indirect_freekick_our_formation;

    //
    // current home positions
    //

    std::vector< PositionType > M_position_types;
    std::vector< rcsc::Vector2D > M_positions;

    // private for singleton
    Strategy();

    // not used
    Strategy( const Strategy & );
    const Strategy & operator=( const Strategy & );
public:

    static
    Strategy & instance();

    static
    const
    Strategy & i()
      {
          return instance();
      }

    //
    // initialization
    //

    bool init( rcsc::CmdLineParser & cmd_parser );
    bool read( const std::string & config_dir );


    //
    // update
    //

    void update( const rcsc::WorldModel & wm );

    //
    // accessor to the current information
    //
    SoccerRole::Ptr createRole( const int number,
                                const rcsc::WorldModel & wm ) const;
    PositionType getPositionType( const int number ) const;
    rcsc::Vector2D getPosition( const int number ) const;



private:
    void updateSituation( const rcsc::WorldModel & wm );
    // update the current position table
    void updatePosition( const rcsc::WorldModel & wm );
    void analyzeOpponentStrategy( const rcsc::WorldModel & wm );

    rcsc::Formation::Ptr readFormation( const std::string & filepath );
    rcsc::Formation::Ptr createFormation( const std::string & type_name ) const;

    rcsc::Formation::Ptr getFormation( const rcsc::WorldModel & wm ) const;

public:
    static
    BallArea get_ball_area( const rcsc::WorldModel & wm );
    static
    BallArea get_ball_area( const rcsc::Vector2D & ball_pos );


//     rcsc::Vector2D getBeforeKickOffPos( const int number ) const;
//     rcsc::Vector2D getPosWhenGoalKickOpp( const int number ) const;
//     rcsc::Vector2D getPosWhenGoalKickOur( const int number ) const;
//     rcsc::Vector2D getPosWhenGoalieCatchOpp( const int number ) const;
//     rcsc::Vector2D getPosWhenGoalieCatchOur( const int number ) const;
//     rcsc::Vector2D getSetPlayPosition( const int number,
//                                        const rcsc::Formation & formation,
//                                        const rcsc::WorldModel & world ) const;

//     rcsc::Formation::Ptr createFormation( const std::string & type_name ) const;

//     boost::shared_ptr<
//         SoccerRole > createRole( const int number,
//                                  const rcsc::WorldModel & world ) const;

// private:
//     rcsc::Formation::Ptr getFormation( const rcsc::WorldModel & world ) const;

//     /////////////////////////////////////////////////////////
//     bool readStaticPositions( const std::string & filepath,
//                               std::vector< rcsc::Vector2D > & positions );

//     /////////////////////////////////////////////////////////
//     rcsc::Formation::Ptr readFormation( const std::string & filepath );

// public:

//     static
//     BallArea get_ball_area( const rcsc::Vector2D & ball_pos );
};

#endif
