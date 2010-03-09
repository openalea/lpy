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

#include "nodemodule.h"
#include <plantgl/python/export_list.h>

using namespace boost::python;
#define bp boost::python
LPY_USING_NAMESPACE


bp::object translate_node(const NodeModule& res) {
	if (!res.isValid()) return bp::object();
	else return boost::python::object(res);
}

bp::object translate_nodes(const std::vector<NodeModule>& res) {
	if (res.empty()) return bp::object();
	else return make_list(res);
}

bp::object py_father(const NodeModule* obj) { return translate_node(obj->father()); }
bp::object py_directSon(const NodeModule* obj) { return translate_node(obj->directSon()); }
bp::object py_sons(const NodeModule* obj) { return translate_nodes(obj->sons()); }
bp::object py_lateralSons(const NodeModule* obj) { return translate_nodes(obj->lateralSons()); }
bp::object py_complex(const NodeModule* obj) { return translate_node(obj->complex()); }
bp::object py_complex1(const NodeModule* obj, int s) { return translate_node(obj->complex(s)); }

void export_NodeModule(){

	class_<NodeModule,  bases<ParamModule> >("NodeModule",no_init)
	.def("father", &py_father)
	.def("sons", &py_sons)
	.def("lateralSons", &py_lateralSons)
	.def("directSon", &py_directSon)
	.def("complex", &py_complex)
	.def("complex", &py_complex1)
	.def("isValid", &NodeModule::isValid)
	.def("isRoot", &NodeModule::isRoot)
	.def("position", &NodeModule::position)
	;

}