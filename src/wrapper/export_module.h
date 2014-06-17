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

#include <plantgl/python/boost_python.h>

template<class ParamModule>
class module_func : public boost::python::def_visitor<module_func<ParamModule> >
{
    friend class boost::python::def_visitor_access;

    template <class classT>
    void visit(classT& c) const
	{
		c.def("argSize", &ParamModule::size)
		 .def("empty",  &ParamModule::empty)
		 .def("hasArg",  &ParamModule::hasArg)
		 .def("__len__", &ParamModule::size)
		 .def("__tuple__",  &ParamModule::toTuple)
		 .def("__getitem__",&ParamModule::getItemAt)
		 .def("__setitem__",&ParamModule::setItemAt)
		 .def("__delitem__",&ParamModule::delItemAt)
		 .def("__getslice__",&ParamModule::getSliceItemAt)
		 .def("__delslice__",&ParamModule::delSliceItemAt)
		 .def("append",&ParamModule::append)
		 .def("prepend",&ParamModule::prepend)

		 .def(boost::python::self == boost::python::self)
		 .def(boost::python::self != boost::python::self)
		 .add_property("args",&ParamModule::getPyArgs,&ParamModule::setPyArgs)
		 .def("getParameterNames",&ParamModule::getParameterNames)
		 .def("getParameterPosition",&ParamModule::getParameterPosition)
		 .def("hasParameter",&ParamModule::hasParameter)
		 .def("getParameter",&ParamModule::getParameter)
		 .def("setParameter",&ParamModule::setParameter)
         .def("getNamedParameterNb",&ParamModule::getNamedParameterNb)
         .def("getNamedParameters",&ParamModule::getNamedParameters,(boost::python::arg("receiver"),boost::python::arg("fromIndex")=0))
        ;
    }
};

