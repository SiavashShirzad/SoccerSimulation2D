// -*-c++-*-

/*
sidious
 */

/////////////////////////////////////////////////////////////////////
#ifndef MARKON
#define MARKON

#include <rcsc/player/soccer_action.h>
#include <vector>
using namespace rcsc;

class MarkOn
	: public rcsc::SoccerBehavior
{
	public:

		bool execute(PlayerAgent *agent);
};

#endif
