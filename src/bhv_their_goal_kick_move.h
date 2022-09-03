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

#ifndef AGENT2D_BHV_THEIR_GOAL_KICK_MOVE_H
#define AGENT2D_BHV_THEIR_GOAL_KICK_MOVE_H

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>

class Bhv_TheirGoalKickMove
    : public rcsc::SoccerBehavior {
private:
    const rcsc::Vector2D M_home_pos;
public:
    Bhv_TheirGoalKickMove( const rcsc::Vector2D & home_pos )
        : M_home_pos( home_pos )
      { }

    bool execute( rcsc::PlayerAgent * agent );

private:
    void doNormal( rcsc::PlayerAgent * agent );
    bool doChaseBall( rcsc::PlayerAgent * agent );
};

#endif
