// -*-c++-*-

/*!
  \file soccer_role.h
  \brief abstract player role class Header File
*/

/*
 *Copyright:

  Copyright (C) Hidehisa AKIYAMA

  This code is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
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

#ifndef AGENT2D_SOCCER_ROLE_H
#define AGENT2D_SOCCER_ROLE_H

#include <rcsc/factory.h>
#include <boost/shared_ptr.hpp>
#include <string>

namespace rcsc {
class PlayerAgent;
}

/*!
  \brief abstruct soccer role
*/
class SoccerRole {
public:

    typedef boost::shared_ptr< SoccerRole > Ptr;
    typedef Ptr (*Creator)();
    typedef rcss::Factory< Creator, std::string > Creators;

    static
    Creators & creators();

    static
    Ptr create( const std::string & name );

private:
    // not used
    SoccerRole( const SoccerRole & );
    SoccerRole & operator=( const SoccerRole & );
protected:

    SoccerRole()
      { }

public:
    //! destructor
    virtual
    ~SoccerRole()
      {
          //std::cerr << "delete SoccerRole" << std::endl;
      }

    //! decide action
    virtual
    void execute( rcsc::PlayerAgent * agent ) = 0;

};

#endif
