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

#ifndef __plot_string_h__
#define __plot_string_h__

#include "lpy_config.h"
#include "stringmatching.h"
#include <plantgl/scenegraph/scene/scene.h>
#include <plantgl/algo/modelling/pglturtle.h>

LPY_BEGIN_NAMESPACE

class AxialTree;

/*---------------------------------------------------------------------------*/

typedef void (*PlotFunction)(const PGL(ScenePtr)&);
typedef std::vector<uint_t> (*GetSelectFunction)();
typedef void (*SaveImageFunction)(const std::string&, const std::string&);
typedef uint_t (*WaitSelectFunction)(const std::string&);
typedef void (*DisplayMessageFunction)(const std::string&);

void LPY_API registerPglPlotFunction(PlotFunction func);
void LPY_API cleanPglPlotFunction();
void LPY_API registerGetSelectionFunction(GetSelectFunction func);
void LPY_API cleanGetSelectionFunction();
void LPY_API registerSaveImageFunction(SaveImageFunction func);
void LPY_API cleanSaveImageFunction();
void LPY_API registerWaitSelectionFunction(WaitSelectFunction func);
void LPY_API cleanWaitSelectionFunction();
void LPY_API registerDisplayMessageFunction(DisplayMessageFunction func);
void LPY_API cleanDisplayMessageFunction();

void LPY_API plot(const PGL(ScenePtr)&);
std::vector<uint_t> LPY_API getSelection();
void saveImage(const std::string&, const std::string& format = "PNG");
uint_t LPY_API waitSelection(const std::string&);
void LPY_API displayMessage(const std::string& txt);

void LPY_API plot(AxialTree& tree);
void LPY_API plot(AxialTree& tree, PGL::PglTurtle& turtle);

void LPY_API plot(AxialTree& tree, StringMatching& matching);
void LPY_API plot(AxialTree& tree, StringMatching& matching, PGL::PglTurtle& turtle);

PGL(ScenePtr) LPY_API scene(AxialTree& tree);
PGL(ScenePtr) LPY_API scene(AxialTree& tree, PGL::PglTurtle& turtle);

PGL(ScenePtr) LPY_API scene(AxialTree& tree, StringMatching& matching);
PGL(ScenePtr) LPY_API scene(AxialTree& tree, StringMatching& matching, PGL::PglTurtle& turtle);

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif


