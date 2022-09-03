// -*-c++-*-

/*
Copyright:


 TORNADO


EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef DFS_MARK_H
#define DFS_MARK_H

#include "defensive_action.h"

#include <rcsc/player/player_agent.h>
#include <rcsc/player/world_model.h>
#include <rcsc/geom/vector_2d.h>

using namespace rcsc;

class Mark : public DefensiveAction
{
    protected:
        Vector2D targetPoint;
    public:
        Mark(PlayerAgent * agentArg);
        ~Mark();
        Vector2D getMarkPoint();
        virtual bool shouldMark()=0;
        virtual bool execute()=0;
};

class PlayOffMark : public Mark
{

    public:
        PlayOffMark(PlayerAgent * agentArg);
        ~PlayOffMark();
        bool shouldMark();
        bool execute();
};

class PlayOnMark : public Mark
{
    private:
        Rect2D getMarkRect();
    public:
        PlayOnMark(PlayerAgent * agentArg);
        ~PlayOnMark();
        bool shouldMark();
        bool execute();
};

#endif
