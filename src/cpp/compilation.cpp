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

void Compilation::setCompiler(eCompiler compiler) {
	if (!cythonAvailable && compiler == eCython) Compiler = eDefaultCompiler;
	else Compiler = compiler;
	if(compiler == eCython){
		tmp_extension = "pyx";
	}
	else {
		tmp_extension = "py";
	}
}

void Compilation::setPythonExec(const std::string& path)
{
	python_exec = path;
}


void Compilation::compile(const std::string& code, PyObject * globals, PyObject * locals, const std::string& fname){
	if(!code.empty()){
		switch(Compiler){
			case eCython:      pyx_file_compile (code, fname, globals, locals); break;
			case ePythonFile : py_file_compile  (code, fname, globals, locals); break;
			default:
			case ePythonStr :  py_string_compile(code, globals, locals); break;
		}
	}
}


Compilation::eCompiler Compilation::Compiler = Compilation::eDefaultCompiler;
bool Compilation::cythonAvailable = true;

std::string Compilation::tmp_extension = "py";
std::string Compilation::python_exec = "python";

void Compilation::py_string_compile(const std::string& code, PyObject * globals, PyObject * locals)
{
	// bp::exec(bp::str(code.c_str()),bp::object(bp::handle<>(bp::borrowed(globals))),bp::object(bp::handle<>(bp::borrowed(locals))));
	bp::handle<>( PyRun_String(code.c_str(),Py_file_input,globals,locals) );
}

std::string Compilation::generate_fname(const std::string& fname)
{
	static size_t tmpid = 0;
	++tmpid;
	if(fname.empty())return "lsystem_"+number(tmpid);
	else return fname+number(tmpid);
}

void Compilation::py_file_write(const std::string& code,
								const std::string& fname)
{
	std::ofstream stream(fname.c_str());
	stream << "from openalea.lpy import *" << std::endl;
	stream << code;
}

void Compilation::py_file_remove(const std::string& fname)
{
	remove(fname.c_str());
}

void Compilation::py_file_import(const std::string& fname, 
								 PyObject * globals,
							     PyObject * locals)
{
	bp::object module = bp::import(fname.c_str());
	bp::handle<> moduledict(bp::borrowed(PyModule_GetDict(module.ptr())));
	if(!moduledict)LsysError("Cannot import module dict");
	PyDict_Update(locals,moduledict.get());	
}

void Compilation::pyx_file_compile(const std::string& code,
								  const std::string& fname, 
								  PyObject * globals,
							      PyObject * locals)
{
	std::string lfname = generate_fname(fname);
	std::string clfname = lfname+"."+tmp_extension;
	py_file_write(code,clfname);
	py_file_import(lfname,globals,locals);
	py_file_remove(clfname);
}

void Compilation::py_file_compile(const std::string& code,
								  const std::string& fname, 
								  PyObject * globals,
							      PyObject * locals)
{
	std::string lfname = generate_fname(fname);
	std::string clfname = lfname+"."+tmp_extension;
	py_file_write(code,clfname);
	std::string cmd = python_exec+" -OO "+lfname+".py";
	system(cmd.c_str());
	py_file_import(lfname,globals,locals);
	py_file_remove(clfname);
}
/*---------------------------------------------------------------------------*/
