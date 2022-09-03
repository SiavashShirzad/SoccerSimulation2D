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





#ifndef AGENT2D_SAMPLE_PLAYER_H
#define AGENT2D_SAMPLE_PLAYER_H

#include <rcsc/player/player_agent.h>

class SamplePlayer
    : public rcsc::PlayerAgent {
public:

    SamplePlayer();

    virtual
    ~SamplePlayer();

protected:

    /*!
      You can override this method.
      But you must call PlayerAgent::initImpl() in this method.
    */
    virtual
    bool initImpl( rcsc::CmdLineParser & cmd_parser );

    //! main decision
    virtual
    void actionImpl();

    //! communication decision
    virtual
    void communicationImpl();

    //
    // hook method for handling server param message
    //
    virtual
    void handleServerParam();
    virtual
    void handlePlayerParam();
    virtual
    void handlePlayerType();
private:

    bool sayBall();
    bool saySelf();
    bool sayGoalie();
    bool sayIntercept();
    bool sayOffsideLine();
    bool sayDefenseLine();

    void attentiontoSomeone();
};

#endif
