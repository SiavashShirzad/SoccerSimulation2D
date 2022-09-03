//siavash

/////////////////////////////////////////////////////////////////////

#ifndef THROUGH_PASS_H
#define THROUGH_PASS_H
#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>
#include <vector>
using namespace std;
using namespace rcsc;

class ThroughPass1 : public rcsc::SoccerBehavior {
private:

double mVel;



Vector2D mpasspoint;
 
int mUnum;

class Passpoint
{
public:
Vector2D point;
double value;
int number;
}; 



 vector <Passpoint> mPasspoints;

public:

    bool execute(PlayerAgent * agent );
    bool get_Passpoint(PlayerAgent * agent,Vector2D*passpoint);
double get_mVel(PlayerAgent * agent);
int get_mUnum(PlayerAgent * agent);
bool canpass(PlayerAgent *agent,Vector2D*inter , int * oppcycle , int * oppcycle1);
//bool canpass1(PlayerAgent *agent,Vector2D*inter ,int * tmmcycle , int * oppcycle1 );  ,int * ballcycle 

};

#endif


