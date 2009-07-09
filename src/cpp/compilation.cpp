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


#include "compilation.h"
#include "error.h"
#include <boost/python.hpp>
#include <plantgl/tool/util_string.h>
#include <fstream>

#define bp boost::python
LPY_USING_NAMESPACE
TOOLS_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

void Compilation::setCythonEnabled(bool enabled) {
	if (enabled) useCython = cythonAvailable;
	else useCython = false;
	if(useCython){
		generatetmpfile = true;
		tmp_extension = "pyx";
		reloadenabled = false;
	}
	else {
		generatetmpfile = false;
		tmp_extension = "py";
		reloadenabled = true;
	}
}


void Compilation::compile(const std::string& code, PyObject * locals, PyObject * globals, const std::string& fname){
	if(!code.empty()){
		if(generatetmpfile) py_file_compile(code,fname,locals,globals);
		else py_compile(code,locals,globals);
	}
}


bool Compilation::useCython = false;
bool Compilation::cythonAvailable = true;

bool Compilation::generatetmpfile = false;
bool Compilation::reloadenabled = true;
std::string Compilation::tmp_extension = "py";

void Compilation::py_compile(const std::string& code, PyObject * globals, PyObject * locals)
{
	bp::handle<>( PyRun_String(code.c_str(),Py_file_input,globals,locals) );
}

std::string Compilation::generate_fname(const std::string& fname)
{
	static size_t tmpid = 0;
	++tmpid;
	if(fname.empty())return "lsystem_"+number(tmpid);
	else return fname+number(tmpid);
}

void Compilation::py_file_compile(const std::string& code,
								  const std::string& fname, 
								  PyObject * globals,
							      PyObject * locals)
{
	std::string lfname = generate_fname(fname);
	std::string clfname = lfname+"."+tmp_extension;
	{
		std::ofstream stream(clfname.c_str());
		stream << "from openalea.lpy import *" << std::endl;
		stream << code;
	}
	bp::object module = bp::import(lfname.c_str());
	bp::handle<> moduledict(bp::borrowed(PyModule_GetDict(module.ptr())));
	if(!moduledict)LsysError("Cannot import module dict");
	PyDict_Update(locals,moduledict.get());
/*
#define MAX_FNAME_LENGTH 500
	assert(fname.size() < MAX_FNAME_LENGTH);
	char cfname[MAX_FNAME_LENGTH];
	strcpy(cfname,lfname.c_str());
	cfname[lfname.size()] = '\0';

	bp::handle<> module (bp::allow_null(PyImport_ImportModuleEx( cfname, globals, locals, NULL)));
	if(!module)LsysError("Cannot import module "+lfname);
	bp::handle<> moduledict(bp::allow_null(PyModule_GetDict(module.get())));
	if(!moduledict)LsysError("Cannot import module dict");
	PyDict_Update(locals,moduledict.get());
	*/
	
}

/*---------------------------------------------------------------------------*/