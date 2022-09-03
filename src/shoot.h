#ifndef SHOOT_H
#define SHOOT_H
#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>
#include <vector>
using namespace std;
using namespace rcsc;
class Shoot : public rcsc::SoccerBehavior {  
private:

double mVel;



Vector2D mShootpoint;
 
int mUnum;

class Shootpoint
{
public:
Vector2D point;
double value;
int number;     
}; 



 vector <Shootpoint> mPasspoints;

public:

    bool execute(PlayerAgent * agent );
    bool get_Shootpoint(PlayerAgent * agent,Vector2D*Shootpoint);
    bool doShoot( PlayerAgent * agent , Vector2D shootpoint);
    bool canShoot(PlayerAgent *agent,Vector2D*Shootpoints , int * oppcycle ,  int *ballcycle);
};

#endif

