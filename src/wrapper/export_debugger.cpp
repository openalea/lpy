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

#include <plantgl/python/pyobj_reference.h>
#include <plantgl/python/pyinterpreter.h>

BOOST_INITIALIZE_WRAPPER_FIX_DECLARE(PyLpyDebugger)

#include <plantgl/python/export_refcountptr.h>
#include "lsystem.h"
using namespace boost::python;
#define bp boost::python

LPY_USING_NAMESPACE

class PyLpyDebugger : public Lsystem::Debugger, public bp::wrapper<Lsystem::Debugger>
{
public:
    PyLpyDebugger() : Lsystem::Debugger(), bp::wrapper<Lsystem::Debugger>(), src(NULL)
      {  }

	virtual void begin(const AxialTree& _src, eDirection dir) {
        PythonInterpreterAcquirer py;
        if (bp::override func = this->get_override("begin")){
			reference_existing_object::apply<AxialTree*>::type lstringconverter;
			bp::call<void>(func.ptr(),object( handle<>(lstringconverter(_src))),bp::object(dir)); 
			src = &_src;
		}
	}

	virtual void end(const AxialTree& result) { 
        PythonInterpreterAcquirer py;
        if (bp::override func = this->get_override("end")){
			reference_existing_object::apply<AxialTree*>::type lstringconverter;
			bp::call<void>(func.ptr(),object( handle<>(lstringconverter(result)))); 
			src = NULL;
		}
	}

	virtual void partial_match(AxialTree::const_iterator match_beg, 
							 AxialTree::const_iterator match_end,
							 const AxialTree& dest, const LsysRule * rule,
							 const ArgList args) 
	{ 
        PythonInterpreterAcquirer py;
        if (bp::override func = this->get_override("partial_match")){
			reference_existing_object::apply<AxialTree*>::type lstringconverter;
			reference_existing_object::apply<LsysRule*>::type ruleconverter;
			bp::call<void>(func.ptr(),bp::object(src->pos(match_beg)),
						   bp::object(src->pos(match_end)),
						   object( handle<>(lstringconverter(dest))),object( handle<>(ruleconverter(rule))),
						   toPyList(args)); 
		}
	}

	virtual void total_match(AxialTree::const_iterator match_beg, 
							 AxialTree::const_iterator match_end,
							 const AxialTree& dest, size_t prodlength,
							 const LsysRule * rule,
							 const ArgList args) 
	{
        PythonInterpreterAcquirer py;
        if (bp::override func = this->get_override("total_match")){
			reference_existing_object::apply<AxialTree*>::type lstringconverter;
			reference_existing_object::apply<LsysRule*>::type ruleconverter;
			bp::call<void>(func.ptr(), bp::object(src->pos(match_beg)),
						   bp::object(src->pos(match_end)),
						   object( handle<>(lstringconverter(dest))),object(prodlength),
						   object( handle<>(ruleconverter(rule))),
						   toPyList(args)); 
		}
	}
	virtual void identity(AxialTree::const_iterator match_pos, 
							 const AxialTree& dest) 
	{
        PythonInterpreterAcquirer py;
        if (bp::override func = this->get_override("identity")){
			reference_existing_object::apply<AxialTree*>::type lstringconverter;
			reference_existing_object::apply<LsysRule*>::type ruleconverter;
			bp::call<void>(func.ptr(),bp::object(src->pos(match_pos)),
						   object( handle<>(lstringconverter(dest)))); 
		}
	}

protected:
	const AxialTree * src;
};

typedef RCPtr<PyLpyDebugger> PyLpyDebuggerPtr;
BOOST_INITIALIZE_WRAPPER_FIX(PyLpyDebugger)

void export_Debugger(){

	class_<PyLpyDebugger,PyLpyDebuggerPtr,boost::noncopyable>
	("LpyDebugger", init<>("LpyDebugger()"))
	.add_property("active",make_getter(&Lsystem::Debugger::active),make_setter(&Lsystem::Debugger::active))
    .def("partial_match", &Lsystem::Debugger::partial_match,&PyLpyDebugger::partial_match)
    .def("total_match", &Lsystem::Debugger::total_match,&PyLpyDebugger::total_match)
    .def("identity", &Lsystem::Debugger::identity,&PyLpyDebugger::identity)
    .def("begin", &Lsystem::Debugger::begin,&PyLpyDebugger::begin)
    .def("end", &Lsystem::Debugger::end,&PyLpyDebugger::end)
	;

}
