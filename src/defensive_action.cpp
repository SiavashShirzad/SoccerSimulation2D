// -*-c++-*-

/*
Copyright:


 TORNADO


EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#include "defensive_action.h"

using namespace rcsc;

DefensiveAction::DefensiveAction(PlayerAgent * agentArg)
{
    agent = agentArg;
    wm = &(agent->world());
}

DefensiveAction::~DefensiveAction()
{
    agent = NULL;
}

