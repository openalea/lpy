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

#include "plot.h"
#include "axialtree.h"

#include <boost/python.hpp>
#include <plantgl/scenegraph/pgl_version.h>
#if PGL_VERSION < 0x020700
#error You should upgrade your version of PlantGL
#endif
#include <plantgl/python/extract_list.h>

using namespace boost::python;
LPY_USING_NAMESPACE
PGL_USING_NAMESPACE

/* ----------------------------------------------------------------------- */

object generateScene(AxialTree& a,PglTurtle& t){
    ScenePtr sc = LPY::scene(a,t);
    return object(sc);
}

object generateScene1(AxialTree& a){
    PglTurtle t;
    return generateScene(a,t);
}


static boost::python::object * MyPlotter = NULL;

static void pyCustomPglPlot(const ScenePtr& sc)
{    
	MyPlotter->attr("plot")(sc);
}

static std::vector<uint_t> pyCustomSelect()
{    
    boost::python::object sel = MyPlotter->attr("selection")(); 
    extract<uint_t> ei(sel);
    if (ei.check()){ // selection is only one id.
        std::vector<uint_t> res;
        res.push_back(ei());
        return res;
    }
    else return extract_vec<uint_t>(sel)();
}

static void pyCustomSave(const std::string& fname, const std::string& format )
{    
	MyPlotter->attr("save")(fname,format);
}

static uint_t pyCustomWaitSelection(const std::string& txt)
{    
	boost::python::object sel = MyPlotter->attr("waitSelection")(txt); 
	if(sel) return extract<uint_t>(sel);
	else return UINT32_MAX;
}

void pyRegisterPlotter(boost::python::object plotter)
{
	if (MyPlotter)delete MyPlotter;
    MyPlotter = new object(plotter);
    registerPglPlotFunction(pyCustomPglPlot);
    registerGetSelectionFunction(pyCustomSelect);
    registerWaitSelectionFunction(pyCustomWaitSelection);
    registerSaveImageFunction(pyCustomSave);
}

void pyCleanPlotter(){
	if (MyPlotter)delete MyPlotter;
	MyPlotter = NULL;
	cleanPglPlotFunction();
	cleanGetSelectionFunction();
	cleanWaitSelectionFunction();
	cleanSaveImageFunction();
}


void export_plot()
{
  def("plot",(void(*)(AxialTree&,PglTurtle&))&LPY::plot);
  def("plot",(void(*)(AxialTree&))&LPY::plot);
  def("generateScene",&generateScene);
  def("generateScene",&generateScene1);
  def("registerPlotter",&pyRegisterPlotter);
  def("cleanPlotter",&pyCleanPlotter);

}
