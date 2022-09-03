// -*-c++-*-

/*
 *Copyright:

 mojtaba jafari

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifndef TOKYOTEC_BHV_BASIC_OFFENSIVE_KICK_H
#define TOKYOTEC_BHV_BASIC_OFFENSIVE_KICK_H
#include<vector>
#include<rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>
using namespace rcsc;
class Tools;
class Block
    : public rcsc::SoccerBehavior {
private:
	rcsc::Vector2D mBlockPoint ;
	class BlockPoint
	{
		public:
		rcsc::Vector2D point;
		double value;
	};
	std::vector<BlockPoint>mBlockPoints;
public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
