// -*-c++-*-

/*
Copyright:


 Mojtaba Jafari


EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef DFS_BLOCK_H
#define DFS_BLOCK_H

#include "defensive_action.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/player/world_model.h>

using namespace rcsc;

class Block : public DefensiveAction
{
    protected:
        Rect2D getBlockRect();
    public :
        Block(PlayerAgent * agentArg);
        ~Block();
        bool execute();
};

#endif
