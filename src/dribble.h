// -*-c++-*-

/*
 *Copyright:

	S.Pegasus

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef DRIBBLE_H
#define DRIBBLE_H
#include <vector>
#include <rcsc/player/soccer_action.h>
using namespace rcsc;

class Dribble
    : public rcsc::SoccerBehavior {
private:

public:
	bool canpass(PlayerAgent *agent,Vector2D*inter , int * oppcycle);
    bool execute( PlayerAgent * agent );
    Vector2D target(PlayerAgent * agent);

};

#endif
