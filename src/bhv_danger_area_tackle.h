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

#ifndef HELIOS_BHV_DANGER_AREA_TACKLE_H
#define HELIOS_BHV_DANGER_AREA_TACKLE_H

#include <rcsc/player/soccer_action.h>

class Bhv_DangerAreaTackle
    : public rcsc::SoccerBehavior {
private:
    const double M_min_probability;
public:
    Bhv_DangerAreaTackle( const double & min_prob = 0.85 )
        : M_min_probability( min_prob )
      { }

    bool execute( rcsc::PlayerAgent * agent );

private:
    bool clearGoal( rcsc::PlayerAgent * agent );
    bool executeOld( rcsc::PlayerAgent * agent );
    bool executeV12( rcsc::PlayerAgent * agent );

};

#endif
