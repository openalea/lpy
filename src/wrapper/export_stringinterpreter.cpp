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

#include "stringinterpreter.h"
#include "axialtree.h"

#include <boost/python.hpp>
using namespace boost::python;
PYLSYS_USING_NAMESPACE
PGL_USING_NAMESPACE

void export_StringInterpreter()
{
  class_< StringInterpreter,boost::noncopyable >("StringInterpreter", init< Turtle* >("StringInterpreter(Turtle)"))
    .def("interpret", (void (StringInterpreter::*)(AxialTree&))&StringInterpreter::interpret )
    .def("interpret", (void (StringInterpreter::*)(ParamModule&))&StringInterpreter::interpret )
    .def("help", &StringInterpreter::help )
    .def("printHelp", &StringInterpreter::printHelp )
    .def("getTurtle", (const PGL::Turtle& (StringInterpreter::*)() const)&StringInterpreter::getTurtle, return_internal_reference<>())
    .def("helpTurtle",(std::string(*)())&PYLSYS::helpTurtle)
    .def("helpTurtle",(std::string(*)(const std::string&))&PYLSYS::helpTurtle)
    .staticmethod("helpTurtle")
    .def("printHelpTurtle",(void(*)())&PYLSYS::printHelpTurtle)
    .def("printHelpTurtle",(void(*)(const std::string&))&PYLSYS::printHelpTurtle)
    .staticmethod("printHelpTurtle")
    ;

  def("helpTurtle",(std::string(*)())&PYLSYS::helpTurtle);
  def("helpTurtle",(std::string(*)(const std::string&))&PYLSYS::helpTurtle);
  def("printHelpTurtle",(void(*)())&PYLSYS::printHelpTurtle);
  def("printHelpTurtle",(void(*)(const std::string&))&PYLSYS::printHelpTurtle);
  def("interpret",(void(*)(AxialTree&,Turtle&))&PYLSYS::interpret);
  def("interpret",(void(*)(AxialTree&))&PYLSYS::interpret);
}
