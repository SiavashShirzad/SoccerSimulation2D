// -*-c++-*-

/*
 *Copyright:

        S.Pegasus

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef OFFENSIVE_ACTION_H
#define OFFENSIVE_ACTION_H

#include <rcsc/player/player_agent.h>
#include <rcsc/player/world_model.h>

using namespace std;
using namespace rcsc;

class OffensiveAction
{
private:
  PlayerAgent *		agent;
  const WorldModel *	wm;

public:
  OffensiveAction	(PlayerAgent * agentArg);
  ~OffensiveAction	();
  virtual bool		execute()=0;

};

#endif
