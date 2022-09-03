// -*-c++-*-

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

#ifndef AGENT2D_SAMPLE_COACH_H
#define AGENT2D_SAMPLE_COACH_H

#include <vector>
#include <map>

#include <rcsc/types.h>

#include <rcsc/coach/coach_agent.h>

namespace rcsc {
class PlayerType;
}

class SampleCoach
    : public rcsc::CoachAgent {
private:
    typedef std::vector< const rcsc::PlayerType * > PlayerTypePtrCont;

    //! used player type id set
    std::vector< int > M_available_player_type_id;

    //! player type assignment mapping. key: unum, value: type id
    std::map< int, int > M_player_type_id;

    //! opponent player type info sent as the known player type
    int M_opponent_player_types[11];

    //! team graphic holder
    rcsc::TeamGraphic M_team_graphic;

public:

    SampleCoach();

    virtual
    ~SampleCoach();

private:

    /*!
      \brief substitute player.

      This methos should be overrided in the derived class
    */
    void doSubstitute();

    void doFirstSubstitute();

    void substituteTo( const int unum,
                       const int type );

    void substituteTo( const std::vector< std::pair< int, int > > & types );

    int getFastestType( PlayerTypePtrCont & candidates );

    /*!
      \brief broadcast opponent players' player types
     */
    void sayPlayerTypes();

    /*!
      \brief send team graphic tiles to rcssserver
     */
    void sendTeamGraphic();

protected:

    /*!
      You can override this method.
      But you must call CoachrAgent::doInit() in this method.
    */
    virtual
    bool initImpl( rcsc::CmdLineParser & cmd_parser );

    //! main decision
    virtual
    void actionImpl();

};

#endif
