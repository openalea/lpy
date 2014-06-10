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

#ifndef __LPY_COMPILATION_H__
#define __LPY_COMPILATION_H__

#include "lpy_config.h"
#include <Python.h>
#include <string>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class LPY_API Compilation {
public:
	enum eCompiler {
		ePythonStr,
		ePythonFile,
		eCython,
		eDefaultCompiler = ePythonStr
	};

	static void setCompiler(eCompiler);
	static inline eCompiler getCompiler() { return Compiler; }

	static void setPythonExec(const std::string& path);
	static inline void setCythonAvailable(bool available) { cythonAvailable = available; }
	static inline bool isCythonAvailable() { return cythonAvailable; }

	static void compile(const std::string& code, PyObject * globals, PyObject * locals, const std::string& fname ="");

protected:
	static eCompiler Compiler;
	static bool cythonAvailable;

	static std::string tmp_extension;
	static std::string python_exec;

	// Compile the code using local and global dict
	static void py_string_compile(const std::string& code, 
						  PyObject * globals, PyObject * locals);

	// Compile the code as a module (in a file) using local and global dict
	static void py_file_compile(const std::string& code, const std::string& fname, 
								PyObject * globals, PyObject * locals);

	static void pyx_file_compile(const std::string& code, const std::string& fname, 
								PyObject * globals, PyObject * locals);

	static std::string generate_fname(const std::string& fname);

	static void py_file_write(const std::string& code, const std::string& fname);
	static void py_file_import(const std::string& fname, PyObject * globals, PyObject * locals);
	static void py_file_remove(const std::string& fname);

};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/

#endif
