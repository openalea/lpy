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
using namespace boost::python;
PYLSYS_USING_NAMESPACE
PGL_USING_NAMESPACE

/* ----------------------------------------------------------------------- */

template<class T, 
	 template < typename > class extractor_t = boost::python::extract, 
	 class result_type = std::vector<T> >
struct extract_vec {

	typedef T element_type;
	typedef extractor_t<T> extractor_type;

	extract_vec(boost::python::object _pylist):pylist(_pylist) {}
	boost::python::object pylist;

	result_type extract() const {
		result_type result;
        if (pylist.ptr() == Py_None) return result;
		boost::python::object iter_obj = boost::python::object( boost::python::handle<>( PyObject_GetIter( pylist.ptr() ) ) );
		while( true )
		{
			boost::python::object obj; 
			try {  obj = iter_obj.attr( "next" )(); }
			catch( boost::python::error_already_set ){ PyErr_Clear(); break; }
			element_type val = extractor_type( obj )();
			result.push_back( val );
		}
		return result;
	}

	inline result_type operator()() const { return extract(); }
	inline operator result_type () const { return extract(); }
};
/* ----------------------------------------------------------------------- */


object generateScene(AxialTree& a,PglTurtle& t){
    ScenePtr sc = PYLSYS::scene(a,t);
    return object(sc);
}

object generateScene1(AxialTree& a){
    PglTurtle t;
    return generateScene(a,t);
}


static boost::python::object * MyPglPlotFunction = NULL;

static void pyCustomPglPlotFunction(const ScenePtr& sc)
{    
    (*MyPglPlotFunction)(sc);
}

void pyRegisterPglPlotFunction(boost::python::object func)
{
	if (MyPglPlotFunction)delete MyPglPlotFunction;
    MyPglPlotFunction = new object(func);
    registerPglPlotFunction(pyCustomPglPlotFunction);
}

void pyCleanPglPlotFunction(){
	if (MyPglPlotFunction)delete MyPglPlotFunction;
	MyPglPlotFunction = NULL;
	cleanPglPlotFunction();
}

static boost::python::object * MySelectFunction = NULL;

static std::vector<uint_t> pyCustomSelectFunction()
{    
    boost::python::object sel = (*MySelectFunction)(); 
    extract<uint_t> ei(sel);
    if (ei.check()){ // selection is only one id.
        std::vector<uint_t> res;
        res.push_back(ei());
        return res;
    }
    else return extract_vec<uint_t>(sel)();
}

void pyRegisterSelectFunction(boost::python::object func)
{
	if (MySelectFunction)delete MySelectFunction;
    MySelectFunction = new object(func);
    registerGetSelectionFunction(pyCustomSelectFunction);
}

void pyCleanGetSelectionFunction(){
	if (MySelectFunction)delete MySelectFunction;
	MySelectFunction = NULL;
	cleanGetSelectionFunction();
}

void export_plot()
{
  def("plot",(void(*)(AxialTree&,PglTurtle&))&PYLSYS::plot);
  def("plot",(void(*)(AxialTree&))&PYLSYS::plot);
  def("generateScene",&generateScene);
  def("generateScene",&generateScene1);
  def("registerPglPlotFunction",&pyRegisterPglPlotFunction);
  def("cleanPglPlotFunction",&pyCleanPglPlotFunction);
  def("registerGetSelectionFunction",&pyRegisterSelectFunction);
  def("cleanGetSelectionFunction",&cleanGetSelectionFunction);

}
