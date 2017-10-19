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

#ifndef __lpy_param_prod_h__
#define __lpy_param_prod_h__

#include "axialtree.h"
#include <plantgl/tool/util_hashmap.h>
#include <queue>

/*---------------------------------------------------------------------------*/

LPY_BEGIN_NAMESPACE

class ParametricProduction;
typedef RCPtr<ParametricProduction> ParametricProductionPtr;

class LPY_API ParametricProduction : public TOOLS(RefCountObject) {
public:
	friend class ParamProductionManager;

	struct ArgPos {
		size_t moduleid;
		size_t argid;
		bool isStarArg;
		bool isNewName;
		ArgPos(size_t m, 
			   size_t a, 
			   bool sa = false, 
			   bool nn = false) : 
		  moduleid(m), argid(a), isStarArg(sa), isNewName(nn) { }
	};

	typedef std::vector<ArgPos> ArgPosList;
	static const size_t modulearg = 1000;

	~ParametricProduction();

	inline void append_variable_module()
	{ 
	  __arguments.push_back(ArgPos(__canvas.size(),modulearg));
	  __canvas.append(ParamModule()); 
	}

	inline void append_module_type(const std::string& name)
	{
		__canvas.append(ParamModule(name)); 
		if(__canvas.last().getClass() == ModuleClass::New)
			next_arg_for_new = true;
	}

	inline void append_module_type(size_t classid) 
	{ 
		__canvas.append(ParamModule(classid)); 
		if(__canvas.last().getClass() == ModuleClass::New)
			next_arg_for_new = true;
	}

	inline void append_module_value(const boost::python::object& value) 
	{ 
	  lpyassert(!__canvas.empty()); 
	  if(next_arg_for_new){
		  __canvas.last().setName(bp::extract<std::string>(value)());
		  next_arg_for_new = false;
	  }
	  else __canvas.last().append(value); 
	}

	inline void append_module_variable() 
	{ 
	   assert(!__canvas.empty()); 
	   __arguments.push_back(ArgPos(__canvas.size()-1,__canvas.last().size(),false,next_arg_for_new));
	   if(next_arg_for_new) next_arg_for_new = false;
	   else __canvas.last().append(bp::object()); 
	}

	inline void append_module_star_variable() 
	{ 
	   assert(!__canvas.empty()); 
	   __arguments.push_back(ArgPos(__canvas.size()-1,__canvas.last().size(),true,next_arg_for_new)); 
	   if(next_arg_for_new) next_arg_for_new = false;
	}

	inline AxialTree generate(const boost::python::list& args) const {
		return generate(0,args);
	}

	inline AxialTree generate(const boost::python::tuple& args) const {
		return generate(1,args);
	}

	inline AxialTree generate(size_t i, const boost::python::object& args) const {
		AxialTree res(__canvas);
		for(ArgPosList::const_iterator itArg = __arguments.begin(); itArg != __arguments.end(); ++itArg, ++i)
			if(itArg->argid != modulearg){
				if(itArg->isStarArg) {
					ParamModule& m = res[itArg->moduleid];
					bp::object argi  = bp::object(args[i]);
					if(itArg->isNewName){
                        bp::extract<bp::dict> pdict(argi);
                        if (pdict.check()) m.setName(bp::extract<std::string>(argi["name"])());
                        else m.setName(bp::extract<std::string>(argi[0])());
                    }
                    m.appendArgumentList(argi);
				}
				else {
					if(itArg->isNewName)res[itArg->moduleid].setName(bp::extract<std::string>(args[i])());
					else res[itArg->moduleid].setAt(itArg->argid,args[i]);
				}
			}
			else res.setAt(itArg->moduleid,bp::extract<ParamModule>(args[i])());
		return res;
	}

	inline size_t nbArgs() const { return __arguments.size(); }
	inline bool hasArgs() const { return !__arguments.empty(); }

	inline AxialTree getCanvas() const { return __canvas; }

	static ParametricProductionPtr create();
	static ParametricProductionPtr get(size_t pid);

	size_t pid() const { return __pid; }

protected:
	ParametricProduction() : next_arg_for_new(false) { }

	AxialTree __canvas;
	ArgPosList __arguments;
	bool next_arg_for_new;
	size_t __pid;
	
};

typedef std::vector<ParametricProductionPtr> ParametricProductionList;

class ParamProductionManager {
	friend class ParametricProduction;
public:

	static ParamProductionManager& get(); 
	~ParamProductionManager();

	ParametricProductionPtr get_production(size_t pid);

protected:
	typedef std::vector<ParametricProduction *> ParametricProductionMap;

	static ParamProductionManager * Instance;

	void add_production(ParametricProduction&);
	void remove_production(ParametricProduction&);

	ParamProductionManager();

	ParametricProductionMap __productions;
	std::queue<size_t> __free_indices;

};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
