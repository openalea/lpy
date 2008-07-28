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
LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

static PlotFunction __PLOT = &PGLViewerApplication::display;

void LPY::registerPglPlotFunction(PlotFunction func)
{
    __PLOT = func;
}

void LPY::cleanPglPlotFunction()
{
    __PLOT = &PGLViewerApplication::display;
}

void LPY::plot(const PGL(ScenePtr)& s)
{
    __PLOT(s);
}

/*---------------------------------------------------------------------------*/

static GetSelectFunction __GETSELECT = &ViewerApplication::getSelection;

void LPY::registerGetSelectionFunction(GetSelectFunction func)
{
    __GETSELECT = func;
}

void LPY::cleanGetSelectionFunction()
{
    __GETSELECT = &PGLViewerApplication::getSelection;
}

std::vector<uint_t> LPY::getSelection()
{
    return __GETSELECT();
}

/*---------------------------------------------------------------------------*/

void LPY::plot(AxialTree& tree){
  plot(tree,LsysContext::currentContext()->turtle);
}

void LPY::plot(AxialTree& tree, PglTurtle& turtle){
  // PGLViewerApplication::display(LPY::scene(tree,turtle));
  __PLOT(LPY::scene(tree,turtle));
}

void LPY::plot(AxialTree& tree,StringMatching& matching){
  plot(tree,matching,LsysContext::currentContext()->turtle);
}

void LPY::plot(AxialTree& tree,StringMatching& matching, PglTurtle& turtle){
  // PGLViewerApplication::display(LPY::scene(tree,turtle));
  __PLOT(LPY::scene(tree,matching,turtle));
}

/*---------------------------------------------------------------------------*/

ScenePtr LPY::scene(AxialTree& tree){
  return scene(tree,LsysContext::currentContext()->turtle);
}

ScenePtr LPY::scene(AxialTree& tree, PglTurtle& turtle){
  interpret(tree,turtle);
  return turtle.getScene();
}

ScenePtr LPY::scene(AxialTree& tree,StringMatching& matching){
  return scene(tree,matching,LsysContext::currentContext()->turtle);
}

ScenePtr LPY::scene(AxialTree& tree,StringMatching& matching, PglTurtle& turtle){
  interpret(tree,turtle,matching);
  return turtle.getScene();
}

/*---------------------------------------------------------------------------*/
