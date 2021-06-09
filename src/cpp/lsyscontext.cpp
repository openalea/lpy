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

#include "lsyscontext.h"
#include "lsysrule.h"
#include "matching.h"
#include "lpy_parser.h"
#include "compilation.h"
#include "tracker.h"
#include <plantgl/version.h>
#include <stack>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

using namespace boost::python;
LPY_USING_NAMESPACE
PGL_USING(PglTurtle)
/*---------------------------------------------------------------------------*/

const std::string LsysContext::InitialisationFunctionName("__initialiseContext__");
const std::string LsysContext::AxiomVariable("__axiom__");
const std::string LsysContext::DerivationLengthVariable("__derivation_length__");
const std::string LsysContext::DecompositionMaxDepthVariable("__decomposition_max_depth__");
const std::string LsysContext::HomomorphismMaxDepthVariable("__homomorphism_max_depth__");

double LsysContext::DefaultAnimationTimeStep(0.05);
const int LsysContext::DEFAULT_OPTIMIZATION_LEVEL(1);

/*---------------------------------------------------------------------------*/

static GlobalContext * GLOBAL_LSYSCONTEXT = NULL;


static std::vector<LsysContext *> LSYSCONTEXT_STACK;
static LsysContext * DEFAULT_LSYSCONTEXT = NULL;
// static LsysContext * CURRENT_LSYSCONTEXT = LsysContext::globalContext();
static LsysContext * CURRENT_LSYSCONTEXT = NULL;
static QMutex CURRENT_LSYSCONTEXT_MUTEX;


size_t func_nb_args(boost::python::object function) {
    const char * attrname =
#if PY_MAJOR_VERSION == 2
     "func_code"
#else
     "__code__"
#endif
     ;
    try {
        return extract<size_t>(function.attr(attrname).attr("co_argcount"))();
    }
    catch (...) { PyErr_Clear(); return 0; }
}
/*
class ContextGarbageCollector
{
public:
	ContextGarbageCollector() {}
	~ContextGarbageCollector() { 
		printf("context garbage collector\n");
		if (GLOBAL_LSYSCONTEXT){
			LsysContext::cleanContexts();
		}
	}
protected:
	static ContextGarbageCollector __INSTANCE;
};
*/

void LsysContext::cleanContexts(){
    // VERY STRANGE: Cannot delete global and default context.
    QMutexLocker locker(&CURRENT_LSYSCONTEXT_MUTEX);
	if (DEFAULT_LSYSCONTEXT){
		// delete DEFAULT_LSYSCONTEXT;
		DEFAULT_LSYSCONTEXT = NULL;
	}
	if (GLOBAL_LSYSCONTEXT)
	{
        if(!(LSYSCONTEXT_STACK.empty() && GLOBAL_LSYSCONTEXT->isCurrent()))
            while(!GLOBAL_LSYSCONTEXT->isCurrent()) currentContext()->done();
        assert(LSYSCONTEXT_STACK.empty() && GLOBAL_LSYSCONTEXT->isCurrent() && "LsysContext not all done!");

        // delete GLOBAL_LSYSCONTEXT;
        GLOBAL_LSYSCONTEXT = NULL;
	}
}

LsysContext *
LsysContext::globalContext()
{ 
    return GlobalContext::get();
}

void createDefaultContext()
{ 
    if(!DEFAULT_LSYSCONTEXT){
        DEFAULT_LSYSCONTEXT = new LocalContext();

		/*
        // copy __builtins__ for import and all.
		DEFAULT_LSYSCONTEXT->copyObjectToGlobal("__builtins__",global);

		if (!DEFAULT_LSYSCONTEXT->hasObject("__builtins__"))
			DEFAULT_LSYSCONTEXT->setObjectToGlobal("__builtins__", object(handle<>(borrowed( PyModule_GetDict(PyImport_AddModule("__builtin__"))))));

        
        // import lpy
        DEFAULT_LSYSCONTEXT->compileInGlobal("from openalea.lpy import *");
        */
   }
}


LsysContext *
LsysContext::defaultContext()
{ 
    if(!DEFAULT_LSYSCONTEXT) createDefaultContext();
    return DEFAULT_LSYSCONTEXT; 
}


LsysContext *
LsysContext::current()
{ 
    QMutexLocker locker(&CURRENT_LSYSCONTEXT_MUTEX);
    if(!CURRENT_LSYSCONTEXT) CURRENT_LSYSCONTEXT = globalContext(); // defaultContext();
	return CURRENT_LSYSCONTEXT; 
}

void 
LsysContext::makeCurrent() 
{ 
  LsysContext * previous = currentContext();
  QMutexLocker locker(&CURRENT_LSYSCONTEXT_MUTEX);
  if (previous == this) {
	  LsysWarning("Multiple activation of same context!");
  }
  LSYSCONTEXT_STACK.push_back(previous);
  CURRENT_LSYSCONTEXT = this;
  previous->pushedEvent(CURRENT_LSYSCONTEXT);
  CURRENT_LSYSCONTEXT->currentEvent();
}

void 
LsysContext::done() 
{ 
  if(isCurrent() && !LSYSCONTEXT_STACK.empty()){
    QMutexLocker locker(&CURRENT_LSYSCONTEXT_MUTEX);
	CURRENT_LSYSCONTEXT = LSYSCONTEXT_STACK.back();
	LSYSCONTEXT_STACK.pop_back();
	doneEvent();
	CURRENT_LSYSCONTEXT->restoreEvent(this);
  }
  else if (this != DEFAULT_LSYSCONTEXT){ LsysError("Not current context trying to be done."); }
}

bool 
LsysContext::isCurrent() const 
{ 
  if (!CURRENT_LSYSCONTEXT) { CURRENT_LSYSCONTEXT = GLOBAL_LSYSCONTEXT; }
  return CURRENT_LSYSCONTEXT == this; 
}


void LsysContext::currentEvent()
{
	for(LsysOptions::iterator it = options.begin(); it != options.end(); ++it)
		(*it)->activateSelection();
	for(ModuleClassList::const_iterator it = __modules.begin(); it != __modules.end(); ++it)
		(*it)->activate();
	for(ModuleVTableList::const_iterator it = __modulesvtables.begin(); it != __modulesvtables.end(); ++it)
		(*it)->activate();
	for(AliasSet::const_iterator it = __aliases.begin(); it != __aliases.end(); ++it)
	    { ModuleClassTable::get().alias(it->first,it->second); }
}

void LsysContext::doneEvent()
{
	for(ModuleClassList::const_iterator it = __modules.begin(); it != __modules.end(); ++it)
		(*it)->desactivate();
	for(ModuleVTableList::const_iterator it = __modulesvtables.begin(); it != __modulesvtables.end(); ++it)
		(*it)->desactivate();
	for(AliasSet::const_iterator it = __aliases.begin(); it != __aliases.end(); ++it)
	    { ModuleClassTable::get().remove(it->first); }
}

void LsysContext::pushedEvent(LsysContext * newEvent)
{
	doneEvent();
}

void LsysContext::restoreEvent(LsysContext * previousEvent)
{
	currentEvent();
}

/*---------------------------------------------------------------------------*/

LsysContext::LsysContext():
__direction(eForward),
__group(0),
__selection_always_required(false),
__selection_requested(false),
__warn_with_sharp_module(true),
__axiom_decomposition_enabled(false),
return_if_no_matching(true),
optimizationLevel(DEFAULT_OPTIMIZATION_LEVEL),
__animation_step(DefaultAnimationTimeStep),
__animation_enabled(false),
__iteration_nb(0),
__nbargs_of_endeach(0),
__nbargs_of_end(0),
__nbargs_of_starteach(0),
__nbargs_of_start(0),
__early_return(false),
__early_return_mutex(),
__paramproductions(),
__multicore(false),
__bracketmapping_optim_level(0)
{
    registerLstringMatcher();
	IncTracker(LsysContext)
	init_options();
}

boost::python::object&
LsysContext::pyturtle() {
    if (_pyturtle == boost::python::object()) _pyturtle = boost::python::object(boost::cref(turtle));
    return _pyturtle;
}

LsysContext::LsysContext(const LsysContext& lsys):
  __direction(lsys.__direction),
  __group(lsys.__group),
  // __nproduction(lsys.__nproduction),
  __selection_always_required(lsys.__selection_always_required),
  __selection_requested(false),
  __warn_with_sharp_module(lsys.__warn_with_sharp_module),
  __axiom_decomposition_enabled(lsys.__axiom_decomposition_enabled),
  return_if_no_matching(lsys.return_if_no_matching),
  optimizationLevel(lsys.optimizationLevel),
  turtle_in_interpretation(false),
  __animation_step(lsys.__animation_step),
  __animation_enabled(lsys.__animation_enabled),
  __iteration_nb(0),
  __nbargs_of_endeach(0),
  __nbargs_of_end(0),
  __nbargs_of_starteach(0),
  __nbargs_of_start(0),
  __early_return(false),
  __early_return_mutex(),
  __paramproductions(),
  __multicore(false),
  __bracketmapping_optim_level(0)
{
    // __nproduction.setLocalData(new AxialTree(*lsys.__nproduction.localData()));
	IncTracker(LsysContext)
	init_options();
}

LsysContext::LsysContext(const boost::python::dict& locals):
__direction(eForward),
__group(0),
__selection_always_required(false),
__selection_requested(false),
__warn_with_sharp_module(true),
__axiom_decomposition_enabled(false),
return_if_no_matching(true),
optimizationLevel(DEFAULT_OPTIMIZATION_LEVEL),
turtle_in_interpretation(false),
__animation_step(DefaultAnimationTimeStep),
__animation_enabled(false),
__iteration_nb(0),
__nbargs_of_endeach(0),
__nbargs_of_end(0),
__nbargs_of_starteach(0),
__nbargs_of_start(0),
__early_return(false),
__early_return_mutex(),
__paramproductions(),
__locals(locals),
__multicore(false),
__bracketmapping_optim_level(0)
{
	IncTracker(LsysContext)
	init_options();
}
LsysContext& 
LsysContext::operator=(const LsysContext& lsys)
{
  __direction = lsys.__direction;
  __group = lsys.__group;
  // __nproduction.setLocalData(lsys.__nproduction.localData());
  // __nproduction = lsys.__nproduction;
  __selection_always_required = lsys.__selection_always_required;
  __selection_requested = false;
  __warn_with_sharp_module = lsys.__warn_with_sharp_module;
  __axiom_decomposition_enabled = lsys.__axiom_decomposition_enabled;
  return_if_no_matching = lsys.return_if_no_matching;
  optimizationLevel = lsys.optimizationLevel;
  turtle_in_interpretation = lsys.turtle_in_interpretation;
  __animation_step =lsys.__animation_step;
  __animation_enabled =lsys.__animation_enabled;
  __nbargs_of_endeach =lsys.__nbargs_of_endeach;
  __nbargs_of_end =lsys.__nbargs_of_end;
  __nbargs_of_starteach =lsys.__nbargs_of_starteach;
  __nbargs_of_start =lsys.__nbargs_of_start;
  __early_return = false;
  __paramproductions = lsys.__paramproductions;
  __bracketmapping_optim_level = lsys.__bracketmapping_optim_level;
  return *this;
}

void 
LsysContext::importContext(const LsysContext& other)
{
	// declareModules(other.declaredModules());
    size_t nb = __modules.size();

	__modules.insert(__modules.end(),other.__modules.begin(),other.__modules.end()); 
	__modulesvtables.insert(__modulesvtables.end(),other.__modulesvtables.begin(),other.__modulesvtables.end()); 

	bool iscurrent = isCurrent();
	for(ModuleClassList::iterator it = __modules.begin()+nb; it != __modules.end(); ++it)
	{
		(*it)->activate(iscurrent);
	}

	add_pproductions(other.get_pproductions());
	updateFromContextNamespace(other);

}


LsysContext::~LsysContext()
{
	DecTracker(LsysContext)
	// std::cerr << "context deleted" << std::endl;
}

void LsysContext::init_options()
{
	LsysOption* option;
	/** module declaration option */
	option = options.add("Module declaration","Specify if module declaration is mandatory.","Compilation");
	option->addValue("On the fly",&ModuleClassTable::setMandatoryDeclaration,false,"Module declaration is made on the fly when parsing string.");
	option->addValue("Mandatory",&ModuleClassTable::setMandatoryDeclaration,true,"Module declaration is mandatory before use in string.");
	option->setDefault(0);
	option->setGlobal(true);
	/** Warning with sharp module option */
	option = options.add("Warning with sharp module","Set whether a warning is made when sharp symbol is met when parsing (compatibility with cpfg).","Compilation");
	option->addValue("Disabled",this,&LsysContext::setWarnWithSharpModule,false,"Disable Warning.");
	option->addValue("Enabled",this,&LsysContext::setWarnWithSharpModule,true,"Enable Warning.");
	option->setDefault(1);	
	/** compiler option */
	option = options.add("Compiler","Specify the compiler to use","Compilation");
	option->addValue("Python",&Compilation::setCompiler,Compilation::ePythonStr,"Use python compiler.");
	option->addValue("Python -OO",&Compilation::setCompiler,Compilation::ePythonFile,"Use python compiler.");
	if(Compilation::isCythonAvailable())
		option->addValue("Cython",&Compilation::setCompiler,Compilation::eCython,"Use Cython compiler.");
	option->setDefault(Compilation::eDefaultCompiler);
	option->setGlobal(true);
	/** optimization option */
	option = options.add("Optimization","Specify the level of optimization to use","Compilation");
	option->addValue("Level 0",this,&LsysContext::setOptimizationLevel,0,"Use Level 0.");
	option->addValue("Level 1",this,&LsysContext::setOptimizationLevel,1,"Use Level 1.");
	option->addValue("Level 2",this,&LsysContext::setOptimizationLevel,2,"Use Level 2.");
	option->setDefault(DEFAULT_OPTIMIZATION_LEVEL);
	/** module matching option */
	option = options.add("Module matching","Specify the way modules are matched to rules pattern","Matching");
	option->addValue("Simple",&MatchingEngine::setModuleMatchingMethod,MatchingEngine::eMSimple,"Simple module matching : Same name and same number of arguments . '*' module allowed.");
	option->addValue("Extension : *args",&MatchingEngine::setModuleMatchingMethod,MatchingEngine::eMWithStar,"Add matching rule that * module can match any module and allow *args to match a number of module arguments");
	option->addValue("Extensions : *args and arg value constraints",&MatchingEngine::setModuleMatchingMethod,MatchingEngine::eMWithStarNValueConstraint,"With * module and *args. A module argument can also be set to a given value adding thus a new matching constraint.");
	option->setDefault(MatchingEngine::eDefaultModuleMatching);
	option->setGlobal(true);
	/** module matching option */
	option = options.add("Module inheritance","Specify if modules inheritance is taken into account in rules pattern matching","Matching");
	option->addValue("Disabled",&MatchingEngine::setInheritanceModuleMatchingActivated,false,"Modules with different names are considered as different even if they inherit from each other.");
	option->addValue("Enabled",&MatchingEngine::setInheritanceModuleMatchingActivated,true,"A module inheriting from a second one can be used for application of the first one if number of parameters are also compatible.");
	option->setDefault(0);
	option->setGlobal(true);
	/** string matching option */
	option = options.add("String matching","Specify the way strings are matched to rules pattern","Matching");
	option->addValue("As String",&MatchingEngine::setStringMatchingMethod,MatchingEngine::eString,"String is considered as a simple string.");
	option->addValue("As AxialTree",&MatchingEngine::setStringMatchingMethod,MatchingEngine::eAxialTree,"String is considered as an axial tree and some modules can be skipped according to tree connectivity.");
	option->addValue("As Multi-level AxialTree",&MatchingEngine::setStringMatchingMethod,MatchingEngine::eMLevelAxialTree,"String is considered as a multi level axial tree and some modules can be skipped according to module level. Level are not ordered.");
	option->addValue("As Multiscale AxialTree",&MatchingEngine::setStringMatchingMethod,MatchingEngine::eMScaleAxialTree,"String is considered as a multi scale axial tree and some modules can be skipped according to module scale.");
	option->setDefault(MatchingEngine::eDefaultStringMatching);
	option->setGlobal(true);
	/** early return when no matching option */
	option = options.add("Early return when no matching","Set whether the L-systems end prematurely if no matching has occured in the last iteration.","Processing");
	option->addValue("Disabled",this,&LsysContext::setReturnIfNoMatching,false,"Disable early return.");
	option->addValue("Enabled",this,&LsysContext::setReturnIfNoMatching,true,"Enable early return.");
	option->setDefault(0);	

    option = options.add("Axiom decomposition","Set whether the axiom is immediatly decomposed.","Processing");
    option->addValue("Disabled",this,&LsysContext::enableAxiomDecomposition,false,"Disable early return.");
    option->addValue("Enabled",this,&LsysContext::enableAxiomDecomposition,true,"Enable early return.");
    option->setDefault(0);  

#if (PGL_VERSION >= 0x020B00)
    /** warn if turtle has invalid value option */
    option = options.add("Warning with Turtle inconsistency","Set whether a warning/error is raised when an invalid value is found during turtle processing.","Processing");
    option->addValue<PglTurtle,bool>("Disabled",&turtle,&PglTurtle::setWarnOnError,false,"Disable warnings/errors.");
    option->addValue<PglTurtle,bool>("Enabled",&turtle,&PglTurtle::setWarnOnError,true,"Enable warnings/errors.");
    option->setDefault(turtle.warnOnError());   
#endif

#if (PGL_VERSION >= 0x021701)
    /** warn if turtle has invalid value option */
    option = options.add("Turtle Path Optimization","Set whether the Turtle use a cache with the path.","Processing");
    option->addValue<PglTurtle,bool>("Disabled",&turtle,&PglTurtle::enablePathInfoCache,false,"Disable Cache.");
    option->addValue<PglTurtle,bool>("Enabled",&turtle,&PglTurtle::enablePathInfoCache,true,"Enable Cache.");
    option->setDefault(turtle.pathInfoCacheEnabled());   
#endif

    /** warn if turtle has invalid value option */
    option = options.add("Turtle in Interpretation rules","Set whether the Turtle is given in the interpretation rules.","Processing");
    option->addValue("Disabled",this,&LsysContext::setTurtleInIntepretation,false,"Disable.");
    option->addValue("Enabled",this,&LsysContext::setTurtleInIntepretation,true,"Enable.");
    option->setDefault(1);   


#ifdef MULTICORE_ENABLED    
    option = options.add("Multicore parallel rewriting","Set whether the string rewriting should be made on multiple cores.","Processing");
    option->addValue("Disabled",this,&LsysContext::setMulticoreProcessing,false,"Disable multicore rewriting.");
    option->addValue("Enabled",this,&LsysContext::setMulticoreProcessing,true,"Enable multicore rewriting.");
    option->setDefault(0);
#endif

    option = options.add("Bracket mapping optimization","Specify the level of optimization of mapping brackets for lstring traversal.","Processing");
    option->addValue("Disabled",this,&LsysContext::setBracketMappingOptimLevel,0,"Disable brackets mapping optimization.");
    option->addValue("On the fly",this,&LsysContext::setBracketMappingOptimLevel,1,"Enable brackets mapping on the fly.");
    option->addValue("As preprocessing",this,&LsysContext::setBracketMappingOptimLevel,2,"Compute brackets mapping as preprocessing of each step.");
    option->setDefault(0);

	/** selection required option */
	option = options.add("Selection Always Required","Set whether selection check in GUI is required or not. Selection is then transform in X module in the Lstring.","Interaction");
	option->addValue("Disabled",this,&LsysContext::setSelectionAlwaysRequired,false,"Disable Selection Check.");
	option->addValue("Enabled",this,&LsysContext::setSelectionAlwaysRequired,true,"Enable Selection Check.");
	option->setDefault(0);
}

void 
LsysContext::clear(){
  __direction = eForward;
  __group = 0;
  __selection_always_required = false;
  __selection_requested = false;
  __iteration_nb = 0;
  __animation_enabled = false;
  __nbargs_of_endeach = 0;
  __nbargs_of_end = 0;
  __nbargs_of_starteach = 0;
  __nbargs_of_start = 0;
  __modules.clear();
  __modulesvtables.clear();
  __aliases.clear();
  __paramproductions.clear();
  __early_return = false;
  clearNamespace();
}

bool
LsysContext::empty( ) const {
  return __modules.empty(); 
}
/*---------------------------------------------------------------------------*/

void 
LsysContext::declare(const std::string& modules)
{
	LpyParsing::ModDeclarationList moduleclasses = LpyParsing::parse_moddeclist(modules);
	bool iscurrent = isCurrent();
	for(LpyParsing::ModDeclarationList::const_iterator it = moduleclasses.begin();
		it != moduleclasses.end(); ++it)
	{
		ModuleClassPtr mod;
		if(!it->alias){
			mod = ModuleClassTable::get().declare(it->name);
			__modules.push_back(mod);
			if(!it->parameters.empty()){
				std::vector<std::string> args = LpyParsing::parse_arguments(it->parameters);
				for(std::vector<std::string>::const_iterator itarg = args.begin(); itarg != args.end(); ++itarg){
					if(!LpyParsing::isValidVariableName(*itarg))LsysError("Invalid parameter name '"+*itarg+"'");
				}
			    mod->setParameterNames(args);
			}
		}
		else mod = ModuleClassTable::get().alias(it->name,it->parameters);
		mod->activate(iscurrent);
	}
}

void LsysContext::declareModules(const ModuleClassList& other) { 
	size_t nb = __modules.size();
	__modules.insert(__modules.end(),other.begin(),other.end()); 
	bool iscurrent = isCurrent();
	for(ModuleClassList::iterator it = __modules.begin()+nb; it != __modules.end(); ++it)
	{
		(*it)->activate(iscurrent);
	}
}

void 
LsysContext::undeclare(const std::string& modules)
{
	LpyParsing::ModNameList moduleclasses = LpyParsing::parse_modlist(modules);
	bool iscurrent = isCurrent();
	for(LpyParsing::ModNameList::const_iterator it = moduleclasses.begin();
		it != moduleclasses.end(); ++it)
	{
		ModuleClassPtr mod = ModuleClassTable::get().getClass(*it);
		undeclare(mod);
	}
}

void 
LsysContext::undeclare(ModuleClassPtr module)
{
	ModuleClassList::iterator it = std::find(__modules.begin(),__modules.end(),module);
	if (it == __modules.end()) LsysError("Cannot undeclare module '"+module->name+"'. Not declared in this scope.");
	else { 
			__modules.erase(it); 
			if(module->__vtable){
				ModuleVTableList::iterator it = 
					find(__modulesvtables.begin(),__modulesvtables.end(),module->__vtable);
				__modulesvtables.erase(it);
			}
			if (isCurrent())module->desactivate(); 
		 }
}


bool LsysContext::isDeclared(const std::string& module)
{
	ModuleClassPtr mod = ModuleClassTable::get().getClass(module);
	if(!mod) return false;
	else return isDeclared(mod);
}

bool LsysContext::isDeclared(ModuleClassPtr module)
{
	ModuleClassList::iterator it = std::find(__modules.begin(),__modules.end(),module);
	return it != __modules.end();
}

void LsysContext::declareAlias(const std::string& alias, ModuleClassPtr module)
{ __aliases[alias] = module; }

void LsysContext::setModuleScale(const std::string& modules, int scale)
{
	LpyParsing::ModNameList moduleclasses = LpyParsing::parse_modlist(modules);
	if (moduleclasses.size() > 0){
		ContextMaintainer cm(this);
		for(LpyParsing::ModNameList::const_iterator it = moduleclasses.begin();
			it != moduleclasses.end(); ++it)
		{
			ModuleClassPtr mod = ModuleClassTable::get().getClass(*it);
			if(mod)mod->setScale(scale);
		}
	}
}

/*---------------------------------------------------------------------------*/

bool 
LsysContext::hasObject(const std::string& name) const{
  if (__locals.has_key(name)) return true;  
  return PyDict_Contains(globals(),object(name).ptr());
}

object
LsysContext::getObject(const std::string& name, const boost::python::object& defaultvalue) const
{
  if (__locals.has_key(name)) return __locals.get(name,defaultvalue);
  handle<> res(allow_null(PyDict_GetItemString(globals(),name.c_str())));
  if(res) return object(handle<>(borrowed(res.get())));
  return defaultvalue;
}

void 
LsysContext::setObject(const std::string& name, 
					   const boost::python::object& o)
{
  __locals[name] = o;
}

void 
LsysContext::setObjectToGlobals(const std::string& name, 
							   const boost::python::object& o)
{
  PyObject * _globals = globals();
  assert(_globals != NULL);
  PyDict_SetItemString(_globals,name.c_str(),o.ptr());
}

void 
LsysContext::delObject(const std::string& name) {
  if (__locals.has_key(name)) __locals[name].del();
  else PyDict_DelItemString(globals(),name.c_str());
}

bool 
LsysContext::copyObject(const std::string& name, LsysContext * sourceContext)
{
	if (!sourceContext) return false;
	boost::python::object obj = sourceContext->getObject(name);
	if (obj == object()) return false;
	__locals[name] = obj;
	return true;
	// PyObject * obj = PyDict_GetItemString(sourceContext->Namespace(),name.c_str());
}


bool 
LsysContext::copyObjectToGlobals(const std::string& name, LsysContext * sourceContext)
{
	if (!sourceContext) return false;
	PyObject * obj = PyDict_GetItemString(sourceContext->locals().ptr(),name.c_str());
	if (obj == NULL) obj = PyDict_GetItemString(sourceContext->globals(),name.c_str());
	if (obj == NULL) return false;
	PyDict_SetItemString(globals(),name.c_str(),obj);
	return true;
}



void
LsysContext::updateNamespace(const boost::python::dict& d){
  PyDict_Update(locals().ptr(),d.ptr());
}

void 
LsysContext::updateFromContextNamespace(const LsysContext& other)
{
  PyDict_Update(locals().ptr(),other.locals().ptr());
  PyDict_Update(globals(),other.globals());
}

void
LsysContext::getNamespace(boost::python::dict& d) const{
	PyDict_Update(d.ptr(),locals().ptr());
}


void
LsysContext::clearNamespace() {
	__locals.clear();
	// PyDict_Clear(globals());
	namespaceInitialisation();
}

void 
LsysContext::namespaceInitialisation()
{
   if (!hasObject("__builtins__")){
        // copyObjectToGlobals("__builtins__",globalContext());

        object builtins = boost::python::import("builtins");
         setObjectToGlobals("__builtins__", builtins);
		// setObjectToGlobals("__builtins__", object(handle<>(borrowed( PyModule_GetDict(PyImport_AddModule("__builtin__"))))));
    }

   if (!hasObject("nproduce")){
       Compilation::compile("from openalea.lpy import *",globals(),globals());
       Compilation::compile("from openalea.plantgl.all import *",globals(),globals());

	   /* handle<>  lpymodule (borrowed( PyModule_GetDict(PyImport_AddModule("openalea.lpy"))));
		PyDict_Update(globals(),lpymodule.get());
		PyDict_DelItemString(globals(),"__file__");
		PyDict_DelItemString(globals(),"__doc__");
		PyDict_DelItemString(globals(),"__path__"); */

   }
}


std::string 
LsysContext::str() const {
  std::stringstream s;
  s << "<LsysContext instance at " << this << " with " << __modules.size() << " declared modules>";
  return s.str();
}

void 
LsysContext::compile(const std::string& code) {
  ContextMaintainer c(this);
  Compilation::compile(code,globals(),locals().ptr());
}

object 
LsysContext::compile(const std::string& name, const std::string& code) {
  if(!code.empty()){
	  Compilation::compile(code,globals(),locals().ptr());
	return getObject(name);
  }
  return object();
}
object 
LsysContext::evaluate(const std::string& code) {
  ContextMaintainer c(this);
  if(!code.empty()){
	dict local_namespace;
	handle<> result(allow_null( 
	  PyRun_String(code.c_str(),Py_eval_input,globals(),local_namespace.ptr())));
	return object(result);
  }
  return object();
}


object 
LsysContext::try_evaluate(const std::string& code) {
  if(!code.empty()){
	dict local_namespace;
	handle<> result(allow_null( 
	  PyRun_String(code.c_str(),Py_eval_input,globals(),local_namespace.ptr())));
	if(result)return object(result);
	else {
	  PyErr_Clear();
	  return object();
	}
  }
  return object();
}



int
LsysContext::readInt(const std::string& code)  {
  std::string::const_iterator b = code.begin();
  while(b != code.end() && (*b == ' ' ||*b == '\t' ||*b == '\n'))b++;
  return extract<int>(evaluate(std::string(b,code.end())));
}

float
LsysContext::readReal(const std::string& code)  {
  std::string::const_iterator b = code.begin();
  while(b != code.end() && (*b == ' ' ||*b == '\t' ||*b == '\n'))b++;
  return extract<float>(evaluate(std::string(b,code.end())));
}


boost::python::object 
LsysContext::start(){
  return func("Start");
}

boost::python::object
LsysContext::start(AxialTree& lstring)
{ return controlMethod("Start",lstring); }


boost::python::object 
LsysContext::end(){
  return func("End");
}

boost::python::object
LsysContext::end(AxialTree& lstring)
{ return controlMethod("End",lstring); }

boost::python::object
LsysContext::end(AxialTree& lstring, const PGL::ScenePtr& scene)
{ return controlMethod("End",lstring,scene); }


boost::python::object  
LsysContext::startEach(){
  return func("StartEach");
}

boost::python::object  
LsysContext::startEach(AxialTree& lstring){
  return controlMethod("StartEach", lstring);
}

void
LsysContext::postDraw(){
  func("PostDraw");
}

boost::python::object 
LsysContext::endEach(){
  return func("EndEach");
}

boost::python::object
LsysContext::endEach(AxialTree& lstring)
{ return controlMethod("EndEach",lstring); }

boost::python::object
LsysContext::endEach(AxialTree& lstring, const PGL::ScenePtr& scene)
{ return controlMethod("EndEach",lstring,scene); }

AxialTree
LsysContext::startInterpretation(boost::python::object pyturtle){
    if(hasStartInterpretationFunction()){
          size_t nbargs = func_nb_args(getObject("StartInterpretation"));
          if (nbargs == 0) func("StartInterpretation");
          else getObject("StartInterpretation")(pyturtle);
          AxialTree nprod = LsysContext::currentContext()->get_nproduction(); 
          if (nprod.empty())  {
            return AxialTree();
          }
          else { 
              LsysContext::currentContext()->reset_nproduction(); // to avoid deep copy
              return nprod;
          }
    }
    return AxialTree();
}


AxialTree
LsysContext::endInterpretation(boost::python::object pyturtle){
    if(hasEndInterpretationFunction()){
          size_t nbargs = func_nb_args(getObject("EndInterpretation"));
          if (nbargs == 0) func("EndInterpretation");
          else getObject("EndInterpretation")(pyturtle);
          AxialTree nprod = LsysContext::currentContext()->get_nproduction(); 
          if (nprod.empty())  return AxialTree();
          else { 
              LsysContext::currentContext()->reset_nproduction(); // to avoid deep copy
              return nprod;
          }
    }
    return AxialTree();
}



boost::python::object
LsysContext::controlMethod(const std::string& name, AxialTree& lstring){
  ContextMaintainer c(this);
  if (hasObject(name)){
	// reference_existing_object::apply<AxialTree*>::type converter;
	// PyObject* obj = converter( &lstring );
	// object real_obj = object( handle<>( obj ) );
	return getObject(name)(object(lstring));
  }
  return object();
}

boost::python::object
LsysContext::controlMethod(const std::string& name, AxialTree& lstring, const PGL::ScenePtr& scene)
{
  ContextMaintainer c(this);
  if (hasObject(name)){
	// reference_existing_object::apply<AxialTree*>::type converter;
	// PyObject* obj = converter( &lstring );
	// object real_obj = object( handle<>( obj ) );
	return getObject(name)(lstring, scene); 
  }
  return object();
}


bool LsysContext::initialise()
{
  ContextMaintainer c(this);
  return __initialise();
}

bool LsysContext::__initialise()
{
  reference_existing_object::apply<LsysContext*>::type converter;
  PyObject* obj = converter( this );
  object real_obj = object( handle<>( obj ) );
  if (hasObject(InitialisationFunctionName)){
	  getObject(InitialisationFunctionName)(real_obj);
	  currentEvent();
	  return true;
  }
  else return false;
}


size_t LsysContext::__initialiseFrom(const std::string& lcode)
{
	ContextMaintainer c(this);
	size_t pos = lcode.find(LpyParsing::InitialisationBeginTag);
	if (pos != std::string::npos) {
		compile(std::string(lcode.begin()+pos,lcode.end()));
		__initialise();
		return pos;
	}
	return std::string::npos;
}

void 
LsysContext::setStart(object func){
  setObject("Start",func);
}

void 
LsysContext::setEnd(object func){
  setObject("End",func);
  check_init_functions();
}

void 
LsysContext::setStartEach(object func){
  setObject("StartEach",func);
}

void 
LsysContext::setEndEach(object func){
  setObject("EndEach",func);
  check_init_functions();
}

void 
LsysContext::setPostDraw(object func){
  setObject("PostDraw",func);
}

void 
LsysContext::setStartInterpretation(object func){
  setObject("StartInterpretation",func);
}

void 
LsysContext::setEndInterpretation(object func){
  setObject("EndInterpretation",func);
}

boost::python::object 
LsysContext::func(const std::string& funcname){
  ContextMaintainer c(this);
  if (hasObject(funcname))return getObject(funcname)();
  return object();
}


void 
LsysContext::check_init_functions()
{
	if (hasObject("StartEach")) {
		__nbargs_of_starteach = func_nb_args(getObject("StartEach"));
    }
	else __nbargs_of_starteach = 0;

	if (hasObject("Start")) {
			__nbargs_of_start = func_nb_args(getObject("Start"));
	}
	else __nbargs_of_start = 0;

	if (hasObject("EndEach")) {
			__nbargs_of_endeach = func_nb_args(getObject("EndEach"));
	}
	else __nbargs_of_endeach = 0;
	if (hasObject("End")) {
			__nbargs_of_end = func_nb_args(getObject("End"));
	}
	else __nbargs_of_end = 0;
}

/*---------------------------------------------------------------------------*/



void LsysContext::nproduce(const AxialTree& prod)
{  
#ifdef PRODUCTION_PER_THREAD
    if (!__nproduction.hasLocalData()) __nproduction.setLocalData(new AxialTree(prod));
    else __nproduction.localData()->append(prod); 
#else
    __nproduction.append(prod);
#endif
}

void LsysContext::reset_nproduction() { 
#ifdef PRODUCTION_PER_THREAD
    if (!__nproduction.hasLocalData())  __nproduction.setLocalData(new AxialTree());
    else __nproduction.localData()->clear();
#else
    __nproduction.clear();
#endif
}

AxialTree LsysContext::get_nproduction() { 
#ifdef PRODUCTION_PER_THREAD
    if (!__nproduction.hasLocalData()) return AxialTree();
    return *__nproduction.localData();
#else
    return __nproduction;
#endif
}

void LsysContext::set_nproduction(const AxialTree& prod) { 
#ifdef PRODUCTION_PER_THREAD
    if (!__nproduction.hasLocalData()) __nproduction.setLocalData(new AxialTree(prod));
    else *__nproduction.localData() = prod; 
#else
     __nproduction = prod;
#endif
}

/*---------------------------------------------------------------------------*/


size_t LsysContext::getIterationNb()
{
    QReadLocker ml(&__iteration_nb_lock);
	return __iteration_nb;
}

void LsysContext::setIterationNb(size_t val)
{
    QWriteLocker ml(&__iteration_nb_lock);
    __iteration_nb = val; 
}

/*---------------------------------------------------------------------------*/

double 
LsysContext::get_animation_timestep()
{
    QReadLocker ml(&__animation_step_mutex);
    return __animation_step;
}

void 
LsysContext::set_animation_timestep(double value)
{
    QWriteLocker ml(&__animation_step_mutex);
    __animation_step = value;
}

bool 
LsysContext::is_animation_timestep_to_default()
{
    QReadLocker ml(&__animation_step_mutex);
    return fabs(__animation_step - DefaultAnimationTimeStep) < GEOM_EPSILON;
}

/*---------------------------------------------------------------------------*/

bool 
LsysContext::isSelectionAlwaysRequired() const
{ return __selection_always_required; }

void 
LsysContext::setSelectionAlwaysRequired(bool enabled)
{ 
	if (__selection_always_required != enabled){
		__selection_always_required = enabled; 
		options.setSelection("Selection Always Required",(size_t)enabled);
	}
}

/*---------------------------------------------------------------------------*/

void 
LsysContext::setWarnWithSharpModule(bool enabled)
{ 
    if (__warn_with_sharp_module != enabled){
        __warn_with_sharp_module = enabled; 
        options.setSelection("Warning with sharp module",(size_t)__warn_with_sharp_module);
    }
}

/*---------------------------------------------------------------------------*/

void 
LsysContext::enableAxiomDecomposition(bool enabled)
{ 
    if (__axiom_decomposition_enabled != enabled){
        __axiom_decomposition_enabled = enabled; 
        options.setSelection("Axiom decomposition",(size_t)__axiom_decomposition_enabled);
    }
}

/*---------------------------------------------------------------------------*/

void LsysContext::enableEarlyReturn(bool val) 
{ 
    QWriteLocker ml(&__early_return_mutex);
    __early_return = val; 
}

bool LsysContext::isEarlyReturnEnabled() 
{ 
    QReadLocker ml(&__early_return_mutex);
    return __early_return; 
}

/*---------------------------------------------------------------------------*/

void LsysContext::registerLstringMatcher(const LstringMatcherPtr& lstringmatcher)
{ 
    // size_t threadid = (size_t)QThread::currentThread();
    
    if(!__lstringmatcher.hasLocalData()) {
        LstringMatcher ** val = new LstringMatcher *(lstringmatcher.get());
        __lstringmatcher.setLocalData(val); 
    }
    else {
        *__lstringmatcher.localData() = lstringmatcher.get();
    }
}

inline bool is_a_null_ptr(LstringMatcher * lm) { return lm == NULL;}

bool LsysContext::pInLeftContext(size_t pid, boost::python::dict& res)
{ 
	if (!__lstringmatcher.hasLocalData() || is_a_null_ptr(*(__lstringmatcher.localData()))) LsysError("Cannot call InLeftContext");
	return (*__lstringmatcher.localData())->pInLeftContext(pid, res);
}

bool LsysContext::inLeftContext(const PatternString& pattern, boost::python::dict& res)
{ 
	if (!__lstringmatcher.hasLocalData() || is_a_null_ptr(*(__lstringmatcher.localData()))) LsysError("Cannot call inLeftContext");
	return (*__lstringmatcher.localData())->inLeftContext(pattern, res);
}


bool LsysContext::pInRightContext(size_t pid, boost::python::dict& res)
{ 
	if (!__lstringmatcher.hasLocalData() || is_a_null_ptr(*(__lstringmatcher.localData()))) LsysError("Cannot call InRightContext");
	return (*__lstringmatcher.localData())->pInRightContext(pid, res);
}

bool LsysContext::inRightContext(const PatternString& pattern, boost::python::dict& res)
{ 
	if (!__lstringmatcher.hasLocalData() || is_a_null_ptr(*(__lstringmatcher.localData()))) LsysError("Cannot call inRightContext");
	return (*__lstringmatcher.localData())->inRightContext(pattern, res);
}


/*---------------------------------------------------------------------------*/

LocalContext::LocalContext(const boost::python::dict& globals):
  LsysContext(globals),
  __globals(globals)
{ 
   namespaceInitialisation();
}

LocalContext::LocalContext(const boost::python::dict& locals, const boost::python::dict& globals):
  LsysContext(locals),
  __globals(globals)
{ 
   namespaceInitialisation();
}

LocalContext::~LocalContext()
{
	if(isCurrent()) done();
	if (!LSYSCONTEXT_STACK.empty()){
		std::vector<LsysContext *>::iterator it = LSYSCONTEXT_STACK.begin();
		for(;it!=LSYSCONTEXT_STACK.end();++it){
			if (*it == this){
				if(it == LSYSCONTEXT_STACK.begin()){
					LSYSCONTEXT_STACK.erase(it);
					it = LSYSCONTEXT_STACK.begin();
				}
				else {
					std::vector<LsysContext *>::iterator it2 = it+1;
					LSYSCONTEXT_STACK.erase(it);
					it = it2;
				}
			}
		}
	}
}


void
LocalContext::clearNamespace() {
  __locals.clear();
  namespaceInitialisation();
}


PyObject * 
LocalContext::globals() const 
{ return __globals.ptr(); }

/*---------------------------------------------------------------------------*/

GlobalContext::GlobalContext():
  LsysContext(){
   __globals =   object(handle<>(borrowed(PyModule_GetDict(PyImport_AddModule("__main__")))));
}

GlobalContext * GlobalContext::get()
{
    if(!GLOBAL_LSYSCONTEXT)  GLOBAL_LSYSCONTEXT = new GlobalContext();
    return GLOBAL_LSYSCONTEXT;    
}


GlobalContext::~GlobalContext()
{
	if(!(LSYSCONTEXT_STACK.empty() && isCurrent()))
		while(!isCurrent()) currentContext()->done();
	assert(LSYSCONTEXT_STACK.empty() && isCurrent() && "LsysContext not all done!");
}


PyObject * 
GlobalContext::globals()  const 
{ return __globals.ptr(); }


boost::python::object GlobalContext::__reprFunc;

boost::python::object bprepr(boost::python::object obj)
{
    return boost::python::object(boost::python::handle<>(PyObject_Repr(obj.ptr())));
}

boost::python::object 
GlobalContext::getFunctionRepr() {
	if(__reprFunc == boost::python::object()){
		__reprFunc =  boost::python::make_function(bprepr);
    }
	return __reprFunc;
}

void
GlobalContext::clearNamespace() {
  __locals.clear();
}
/*---------------------------------------------------------------------------*/

  