/* ---------------------------------------------------------------------------
 #
 #       L-Py: L-systems in Python
 #
 #       Copyright 2003-2008 UMR Cirad/Inria/Inra Dap - Virtual Plant Team
 #
 #       File author(s): F. Boudon (frederic.boudon@cirad.fr)
 #
 # ---------------------------------------------------------------------------
 #
 #                      GNU General Public Licence
 #
 #       This program is free software; you can redistribute it and/or
 #       modify it under the terms of the GNU General Public License as
 #       published by the Free Software Foundation; either version 2 of
 #       the License, or (at your option) any later version.
 #
 #       This program is distributed in the hope that it will be useful,
 #       but WITHOUT ANY WARRANTY; without even the implied warranty of
 #       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
 #       GNU General Public License for more details.
 #
 #       You should have received a copy of the GNU General Public
 #       License along with this program; see the file COPYING. If not,
 #       write to the Free Software Foundation, Inc., 59
 #       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 #
 # ---------------------------------------------------------------------------
 */

#ifndef __PGL_STRINGINTERPRETER_H__
#define __PGL_STRINGINTERPRETER_H__

#include "pylsys_config.h"
#include "stringmatching.h"
#include <vector>
#include <string>
#include <plantgl/algo/modelling/turtle.h>

PYLSYS_BEGIN_NAMESPACE

class ParamModule;
class AxialTree;

/*---------------------------------------------------------------------------*/

class PYLSYS_API StringInterpreter {
public:

    StringInterpreter(PGL::Turtle* turtle);
    ~StringInterpreter();

	const PGL::Turtle& getTurtle() const;
	PGL::Turtle& getTurtle();
	void setTurtle(PGL::Turtle* turtle);
    
    std::string help(const std::string& command);
    void printHelp(const std::string& command);

	void interpret(ParamModule& mod);
		
    void interpret(AxialTree& tree);   
    void interpret(AxialTree& tree, const StringMatching& matching);   

protected:

    void initCommand();

    void addCommand(const std::string& command,
					const std::string& comment);

	PGL::Turtle * __turtle;
  
	std::vector<std::pair<std::string,std::string> > __comments;
};

/*---------------------------------------------------------------------------*/


std::string PYLSYS_API helpTurtle(const std::string& command );
std::string PYLSYS_API helpTurtle() ;

void PYLSYS_API printHelpTurtle(const std::string& command );
void PYLSYS_API printHelpTurtle() ;

void PYLSYS_API interpret(AxialTree& tree);
void PYLSYS_API interpret(AxialTree& tree, PGL::Turtle& turtle);

/*---------------------------------------------------------------------------*/

PYLSYS_END_NAMESPACE

#endif
