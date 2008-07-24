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

#include "plot.h"
#include "axialtree.h"
#include "stringinterpreter.h"
#include "lsyscontext.h"
#include <plantgl/gui/viewer/pglapplication.h>


PGL_USING_NAMESPACE
TOOLS_USING_NAMESPACE
PYLSYS_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

static PlotFunction __PLOT = &PGLViewerApplication::display;

void PYLSYS::registerPglPlotFunction(PlotFunction func)
{
    __PLOT = func;
}

void PYLSYS::cleanPglPlotFunction()
{
    __PLOT = &PGLViewerApplication::display;
}

void PYLSYS::plot(const PGL(ScenePtr)& s)
{
    __PLOT(s);
}

/*---------------------------------------------------------------------------*/

static GetSelectFunction __GETSELECT = &ViewerApplication::getSelection;

void PYLSYS::registerGetSelectionFunction(GetSelectFunction func)
{
    __GETSELECT = func;
}

void PYLSYS::cleanGetSelectionFunction()
{
    __GETSELECT = &PGLViewerApplication::getSelection;
}

std::vector<uint_t> PYLSYS::getSelection()
{
    return __GETSELECT();
}

/*---------------------------------------------------------------------------*/

void PYLSYS::plot(AxialTree& tree){
  plot(tree,LsysContext::currentContext()->turtle);
}

void PYLSYS::plot(AxialTree& tree, PglTurtle& turtle){
  // PGLViewerApplication::display(PYLSYS::scene(tree,turtle));
  __PLOT(PYLSYS::scene(tree,turtle));
}

void PYLSYS::plot(AxialTree& tree,StringMatching& matching){
  plot(tree,matching,LsysContext::currentContext()->turtle);
}

void PYLSYS::plot(AxialTree& tree,StringMatching& matching, PglTurtle& turtle){
  // PGLViewerApplication::display(PYLSYS::scene(tree,turtle));
  __PLOT(PYLSYS::scene(tree,matching,turtle));
}

/*---------------------------------------------------------------------------*/

ScenePtr PYLSYS::scene(AxialTree& tree){
  return scene(tree,LsysContext::currentContext()->turtle);
}

ScenePtr PYLSYS::scene(AxialTree& tree, PglTurtle& turtle){
  StringInterpreter s(&turtle);
  s.interpret(tree);
  return turtle.getScene();
}

ScenePtr PYLSYS::scene(AxialTree& tree,StringMatching& matching){
  return scene(tree,matching,LsysContext::currentContext()->turtle);
}

ScenePtr PYLSYS::scene(AxialTree& tree,StringMatching& matching, PglTurtle& turtle){
  StringInterpreter s(&turtle);
  s.interpret(tree,matching);
  return turtle.getScene();
}

/*---------------------------------------------------------------------------*/
