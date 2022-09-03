/*

TORNADO

*/
#ifndef TORNADO_LOGGER_H
#define TORNADO_LOGGER_H



#include <cstdlib>
#include <cstdio>
#include <rcsc/geom/vector_2d.h>
#include <iostream>
#include <rcsc/player/player_agent.h>

static FILE * fp=NULL;
static bool loggerIsEnabled = false;
static int lastCycle=0;
static rcsc::PlayerAgent * agent=NULL;
static void enableLogger (rcsc::PlayerAgent * agentArg)
{
    int unum = agentArg->world().self().unum();
    lastCycle = agentArg->world().time().cycle();
    agent = agentArg;
    char * str="../logs/UNKNOWN_PLAYER.txt";
	switch (unum)
	{
        case 1:
            str = "../logs/player_1.txt";
            break;
        case 2:
            str = "../logs/player_2.txt";
            break;
        case 3:
            str = "../logs/player_3.txt";
            break;
        case 4:
            str = "../logs/player_4.txt";
            break;
        case 5:
            str = "../logs/player_5.txt";
            break;
        case 6:
            str = "../logs/player_6.txt";
            break;
        case 7:
            str = "../logs/player_7.txt";
            break;
        case 8:
            str = "../logs/player_8.txt";
            break;
        case 9:
            str = "../logs/player_9.txt";
            break;
        case 10:
            str = "../logs/player_10.txt";
            break;
        case 11:
            str = "../logs/player_11.txt";
            break;
	}
	fp=fopen(str,"w");

    if(fp==NULL)
    {
        std::cout<<"\n can't open file \n"<< unum;
    }
    else
        loggerIsEnabled = true;
}

void checkLogger(rcsc::PlayerAgent * agentArg)
{
    if(!loggerIsEnabled)
    {
        enableLogger(agentArg);
    }
}

void checkCycle()
{
    if(loggerIsEnabled && agent && !(lastCycle == agent->world().time().cycle()))
     {
         lastCycle =agent->world().time().cycle();
         fprintf(fp , "\n \n _______________________________ CYCLE[%d] ______________________________ \n \n", lastCycle);
     }
}

void logText (const char * str)
{
    if(loggerIsEnabled)
    {
        checkCycle();
        fprintf(fp , "\n %s ", str);
        //fputs(str,fp);
        //fprintf( fp , "\n my unum is %d " , agent->world().self().unum() );
    }

}

void logVector( rcsc::Vector2D vec)
{
        if(loggerIsEnabled)
    {
        checkCycle();
        fprintf( fp , "  ( %f , %f )  " , vec.x ,vec.y );
    }
}

#endif
