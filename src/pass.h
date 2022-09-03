//siavash

/////////////////////////////////////////////////////////////////////

#ifndef PASS_H
#define PASS_H
#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>
#include <vector>
using namespace std;
using namespace rcsc;

class Pass : public rcsc::SoccerBehavior {  
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
    bool doPass( PlayerAgent * agent , Vector2D passpos);
bool get_Passpoint(PlayerAgent * agent,Vector2D*passpoint);
double get_mVel(PlayerAgent * agent);
int get_mUnum(PlayerAgent * agent);
bool canpass(PlayerAgent *agent,Vector2D*inter , int * oppcycle);
};

#endif






















