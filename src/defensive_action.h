// -*-c++-*-

/*
Copyright:


 TORNADO


EndCopyright:
 */

/////////////////////////////////////////////////////////////////////


#ifndef DEFENSIVE_ACTION_H
#define DEFENSIVE_ACTION_H

#include <rcsc/player/player_agent.h>
#include <rcsc/player/world_model.h>

using namespace rcsc;

class DefensiveAction
{
 protected:
    PlayerAgent *     agent;
    const WorldModel * wm;
 public:
    DefensiveAction     (PlayerAgent * agentArg);
    ~DefensiveAction    ();
    virtual bool execute()=0;
};

#endif
