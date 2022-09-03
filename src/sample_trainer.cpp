// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "sample_trainer.h"

#include <rcsc/trainer/trainer_command.h>
#include <rcsc/trainer/trainer_config.h>
#include <rcsc/coach/global_world_model.h>
#include <rcsc/common/basic_client.h>
#include <rcsc/common/player_param.h>
#include <rcsc/common/player_type.h>
#include <rcsc/common/server_param.h>
#include <rcsc/param/param_map.h>
#include <rcsc/param/cmd_line_parser.h>
#include <rcsc/random.h>

/*-------------------------------------------------------------------*/
/*!

 */
SampleTrainer::SampleTrainer()
    : TrainerAgent()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
SampleTrainer::~SampleTrainer()
{

}

/*-------------------------------------------------------------------*/
/*!

 */
bool
SampleTrainer::initImpl( rcsc::CmdLineParser & cmd_parser )
{
    bool result = rcsc::TrainerAgent::initImpl( cmd_parser );

    rcsc::ParamMap my_params;
#if 0
    std::string formation_conf;
    my_map.add()
        ( &conf_path, "fconf" )
        ;
#endif
    cmd_parser.parse( my_params );

    if ( cmd_parser.count( "help" ) )
    {
        my_params.printHelp( std::cout );
        return false;
    }

    if ( cmd_parser.failed() )
    {
        std::cerr << "trainer: ***WARNING*** detected unsupported options: ";
        cmd_parser.print( std::cerr );
        std::cerr << std::endl;
    }

    if ( ! result )
    {
        return false;
    }

    //////////////////////////////////////////////////////////////////
    // Add your code here.
    //////////////////////////////////////////////////////////////////


    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
SampleTrainer::actionImpl()
{
    //////////////////////////////////////////////////////////////////
    // This function is a sample trainer action.
    // At first, remove this.
    sampleAction();

    // Add your code here.
    //////////////////////////////////////////////////////////////////
}


/*-------------------------------------------------------------------*/
/*!

 */
void
SampleTrainer::sampleAction()
{
    // sample training to test a ball interception.

    static int s_state = 0;
    static int s_wait_counter = 0;

    static rcsc::Vector2D s_last_player_move_pos;

    if ( world().existKickablePlayer() )
    {
        s_state = 1;
    }

    switch ( s_state ) {
    case 0:
        // nothing to do
        break;
    case 1:
        // exist kickable left player

        // recover stamina
        doRecover();
        // move ball to center
        doMoveBall( rcsc::Vector2D( 0.0, 0.0 ),
                    rcsc::Vector2D( 0.0, 0.0 ) );
        // change playmode to play_on
        doChangeMode( rcsc::PM_PlayOn );
        {
            // move player to random point
            rcsc::UniformReal uni01( 0.0, 1.0 );
            rcsc::Vector2D move_pos
                = rcsc::Vector2D::polar2vector( 20.0,
                                                rcsc::AngleDeg( 360.0 * uni01() ) );
            s_last_player_move_pos = move_pos;

            doMovePlayer( config().teamName(),
                          1, // uniform number
                          move_pos,
                          move_pos.th() - 180.0 );
        }
        doSay( "move player" );
        s_state = 2;
        std::cout << "trainer: actionImpl set ball center" << std::endl;
        break;
    case 2:
        ++s_wait_counter;
        if ( s_wait_counter > 3
             && ! world().playersLeft().empty() )
        {
            // add velocity to the ball
            rcsc::UniformReal uni_spd( 2.0, rcsc::ServerParam::i().ballSpeedMax() );
            rcsc::UniformReal uni_ang( -50.0, 50.0 );
            rcsc::Vector2D velocity
                = rcsc::Vector2D::polar2vector( uni_spd(),
                                                s_last_player_move_pos.th()
                                                + uni_ang() );
            doMoveBall( rcsc::Vector2D( 0.0, 0.0 ),
                        velocity );
            s_state = 0;
            s_wait_counter = 0;
            std::cout << "trainer: actionImpl start ball" << std::endl;
        }
        break;

    }
}
