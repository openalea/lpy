/* 
# ---------------------------------------------------------------------------
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

#include "mod.h"
#include "axialtree.h"
#include "interpretation.h"
#include "lsyscontext.h"
#include "tracker.h"
#include <sstream>

PGL_USING_NAMESPACE
TOOLS_USING_NAMESPACE
LPY_USING_NAMESPACE


/*
std::string StringInterpreter::help(const std::string& command){
    std::stringstream stream;
  if(command.empty()){
	for (std::vector<std::pair<std::string,std::string> >::const_iterator i =__comments.begin();
	i != __comments.end(); i++)
	  stream << i->first << " : "<< i->second << std::endl;
  }
  else{
	std::vector<std::pair<std::string,std::string> >::const_iterator i =__comments.begin();
	for (;i != __comments.end() && i->first != command ;)i++;
	if (i !=  __comments.end())
	  stream << i->first << " : "<< i->second << std::endl;
  }
  return stream.str();
}*/




/*---------------------------------------------------------------------------*/

std::string LPY::helpTurtle(const std::string& command){
  ModuleClassPtr m = ModuleClassTable::get().findClass(command);
  if (m) return m->getDocumentation();
  else return "";
}

/*---------------------------------------------------------------------------*/

std::string LPY::helpTurtle() { 
    std::stringstream stream;
	for(ModuleClassList::const_iterator it = ModuleClass::getPredefinedClasses().begin();
		it != ModuleClass::getPredefinedClasses().end(); ++it) 
	{
	  const ModuleClassPtr& m = *it;
	  if (m == ModuleClass::None) continue;
	  stream << m->name;
	  if (!m->aliases.empty()){
		  stream << " ( ";
		  for(std::vector<std::string>::const_iterator ita = m->aliases.begin();
			  ita != m->aliases.end(); ++ita){
			if (ita != m->aliases.begin()) stream << ", ";
			stream << *ita;
		  }
		  stream << " )";
	  }
	  stream << " : "<< m->getDocumentation() << std::endl;
	}
	return stream.str();
}


/*---------------------------------------------------------------------------*/

void LPY::interpret(AxialTree& tree, Turtle& turtle){
  turtle.start();
  turtle.setId(0);
  for(AxialTree::iterator _it = tree.begin();
      _it != tree.end(); ++_it) {
        _it->interpret(turtle);
        if(turtle.getId() != Shape::NOID)turtle.incId();
  }
  turtle.stop();
  if (!turtle.emptyStack()){
      printf("Turtle stack size : %i\n",turtle.getStack().size());
	  LsysError("Ill-formed string: unmatched brackets");
  }
}

void LPY::interpret(AxialTree& tree, Turtle& turtle, const StringMatching& matching){
  turtle.start();
  StringMatching::const_iterator _iditer = matching.begin();
  turtle.setId(*_iditer);
  for(AxialTree::iterator _it = tree.begin();_it != tree.end(); ++_it,++_iditer){
        _it->interpret(turtle);
        if(turtle.getId() != Shape::NOID)turtle.setId(*_iditer);
  }
  turtle.stop();
  if (!turtle.emptyStack()){
      printf("Turtle stack size : %i\n",turtle.getStack().size());
	LsysError("Ill-formed string: unmatched brackets");
  }
}

/*---------------------------------------------------------------------------*/


