// -*-c++-*-

/*
 *Copyright:

        S.Pegasus

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#include "offensive_action.h"

using namespace std;
using namespace rcsc;

OffensiveAction::OffensiveAction(PlayerAgent* agentArg)
{
  agent = agentArg;
  wm = &(agent -> world());
}

OffensiveAction::~OffensiveAction()
{
  agent = NULL;
}

