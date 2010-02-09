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

bool Module::isConsidered() const
{ 
	// return LsysContext::currentContext()->isConsidered(__name); 
	return LsysContext::currentContext()->isConsidered(*this); 
}

bool Module::isIgnored() const
{ 
	// return LsysContext::currentContext()->isIgnored(__name); 
	return LsysContext::currentContext()->isIgnored(*this); 
}

/*---------------------------------------------------------------------------*/

static GlobalContext * GLOBAL_LSYSCONTEXT = NULL;


static std::vector<LsysContext *> LSYSCONTEXT_STACK;
static LsysContext * DEFAULT_LSYSCONTEXT = NULL;
// static LsysContext * CURRENT_LSYSCONTEXT = LsysContext::globalContext();
static LsysContext * CURRENT_LSYSCONTEXT = NULL;

class ContextGarbageCollector
{
public:
	ContextGarbageCollector() {}
	~ContextGarbageCollector() { 
		// std::cerr  << "context garbage collector" << std::endl;
		if (GLOBAL_LSYSCONTEXT){
			LsysContext::cleanContexts();
		}
	}
protected:
	static ContextGarbageCollector __INSTANCE;
};

void LsysContext::cleanContexts(){
    // std::cerr  << "contexts deletion" << std::endl;
	if (DEFAULT_LSYSCONTEXT){
		delete DEFAULT_LSYSCONTEXT;
		DEFAULT_LSYSCONTEXT = NULL;
	}
	if (GLOBAL_LSYSCONTEXT)
	{
		delete GLOBAL_LSYSCONTEXT;
		GLOBAL_LSYSCONTEXT = NULL;
	}
}

LsysContext *
LsysContext::global()
{ 
    if(!GLOBAL_LSYSCONTEXT)  GLOBAL_LSYSCONTEXT = new GlobalContext();
 	// if(!GLOBAL_LSYSCONTEXT->hasObject("pproduce"))
	//	GLOBAL_LSYSCONTEXT->LsysContext::compile("from openalea.lpy import *");
	return GLOBAL_LSYSCONTEXT; 
}

void createDefaultContext()
{ 
    if(!DEFAULT_LSYSCONTEXT){
		LsysContext * global =  LsysContext::globalContext();
        DEFAULT_LSYSCONTEXT = new LocalContext(false);
        // copy __builtins__ for import and all.
		DEFAULT_LSYSCONTEXT->copyObject("__builtins__",global);
		DEFAULT_LSYSCONTEXT->copyObject("__doc__",global);
        // import pylsystems
        DEFAULT_LSYSCONTEXT->compile("from openalea.lpy import *");
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
    if(!CURRENT_LSYSCONTEXT) CURRENT_LSYSCONTEXT = global(); // defaultContext();
	return CURRENT_LSYSCONTEXT; 
}

void 
LsysContext::makeCurrent() 
{ 
  LsysContext * previous = currentContext();
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
__ignore_method(true),
__direction(eForward),
__group(0),
__selection_required(false),
__warn_with_sharp_module(true),
return_if_no_matching(true),
optimizationLevel(DEFAULT_OPTIMIZATION_LEVEL),
__animation_step(DefaultAnimationTimeStep),
__animation_enabled(false),
__iteration_nb(0),
__nbargs_of_endeach(0),
__nbargs_of_end(0),
__early_return(false),
__early_return_mutex(),
__paramproductions()
{
	IncTracker(LsysContext)
	init_options();
}

LsysContext::LsysContext(const LsysContext& lsys):
  __keyword(lsys.__keyword),
  __ignore_method(lsys.__ignore_method),
  __direction(lsys.__direction),
  __group(lsys.__group),
  __nproduction(lsys.__nproduction),
  __selection_required(lsys.__selection_required),
  __warn_with_sharp_module(lsys.__warn_with_sharp_module),
  return_if_no_matching(lsys.return_if_no_matching),
  optimizationLevel(lsys.optimizationLevel),
  __animation_step(lsys.__animation_step),
  __animation_enabled(lsys.__animation_enabled),
  __iteration_nb(0),
  __nbargs_of_endeach(0),
  __nbargs_of_end(0),
  __early_return(false),
  __early_return_mutex(),
  __paramproductions()
{
	IncTracker(LsysContext)
	init_options();
}

LsysContext& 
LsysContext::operator=(const LsysContext& lsys)
{
  __keyword = lsys.__keyword;
  __ignore_method =lsys.__ignore_method;
  __direction = lsys.__direction;
  __group = lsys.__group;
  __nproduction = lsys.__nproduction;
  __selection_required = lsys.__selection_required;
  __warn_with_sharp_module = lsys.__warn_with_sharp_module;
  return_if_no_matching = lsys.return_if_no_matching;
  optimizationLevel = lsys.optimizationLevel;
  __animation_step =lsys.__animation_step;
  __animation_enabled =lsys.__animation_enabled;
  __nbargs_of_endeach =lsys.__nbargs_of_endeach;
  __nbargs_of_end =lsys.__nbargs_of_end;
  __early_return = false;
  __paramproductions = lsys.__paramproductions;
  return *this;
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
	option->setDefault(1);	
#if (PGL_VERSION >= 0x020B00)
	/** warn if turtle has invalid value option */
	option = options.add("Warning with Turtle inconsistency","Set whether a warning/error is raised when an invalid value is found during turtle processing.","Processing");
	option->addValue<PglTurtle,bool>("Disabled",&turtle,&PglTurtle::setWarnOnError,false,"Disable warnings/errors.");
	option->addValue<PglTurtle,bool>("Enabled",&turtle,&PglTurtle::setWarnOnError,true,"Enable warnings/errors.");
	option->setDefault(turtle.warnOnError());	
#endif
	/** selection required option */
	option = options.add("Selection Required","Set whether selection check in GUI is required or not. Selection is then transform in X module in the Lstring.","Interaction");
	option->addValue("Disabled",this,&LsysContext::setSelectionRequired,false,"Disable Selection Check.");
	option->addValue("Enabled",this,&LsysContext::setSelectionRequired,true,"Enable Selection Check.");
	option->setDefault(0);
}

void 
LsysContext::clear(){
  __keyword.clear();
  __ignore_method = true;
  __direction = eForward;
  __group = 0;
  __selection_required = false;
  __iteration_nb = 0;
  __animation_enabled = false;
  __nbargs_of_endeach = 0;
  __nbargs_of_end = 0;
  __modules.clear();
  __modulesvtables.clear();
  __aliases.clear();
  __paramproductions.clear();
  __early_return = false;
  clearNamespace();
}

bool
LsysContext::empty( ) const {
  return __keyword.empty();
}

/*---------------------------------------------------------------------------*/

void
LsysContext::consider(const std::string& modules){
  __ignore_method = false;
  __keyword.clear();
  if(!modules.empty()){
	AxialTree t(modules);
	for(AxialTree::const_iterator _it = t.begin(); _it != t.end(); ++_it)
	  __keyword[_it->getClass()->getId()] = _it->getClass();
  }
}

void
LsysContext::ignore(const std::string& modules){
  __ignore_method = true;
  __keyword.clear();
  if(!modules.empty()){
	AxialTree t(modules);
	for(AxialTree::const_iterator _it = t.begin(); _it != t.end(); ++_it)
	  __keyword[_it->getClass()->getId()] = _it->getClass();
  }
}

bool
LsysContext::isConsidered(const std::string& module) const{
  if(__keyword.empty())return true;
  ModuleClassSet::const_iterator _it = __keyword.find(ModuleClassTable::get().getClass(module)->getId());
  if(__ignore_method) return _it == __keyword.end();
  else return _it != __keyword.end();
}

bool
LsysContext::isIgnored(const std::string& module) const{
  if(__keyword.empty())return false;
  ModuleClassSet::const_iterator _it = __keyword.find(ModuleClassTable::get().getClass(module)->getId());
  if(__ignore_method) return _it != __keyword.end();
  else return _it == __keyword.end();
}

bool
LsysContext::isConsidered(const Module& module) const{
  if(__keyword.empty())return false;
  ModuleClassSet::const_iterator _it = __keyword.find(module.getClass()->getId());
  if(__ignore_method) return _it == __keyword.end();
  else return _it != __keyword.end();
}

bool
LsysContext::isIgnored(const Module& module) const{
  if(__keyword.empty())return false;
  ModuleClassSet::const_iterator _it = __keyword.find(module.getClass()->getId());
  if(__ignore_method) return _it != __keyword.end();
  else return _it == __keyword.end();
}

std::string
LsysContext::keyword() const{
  if(__keyword.empty())return std::string("");
  std::string res;
  for(ModuleClassSet::const_iterator _it = __keyword.begin();
	  _it != __keyword.end(); ++_it)
	  res += _it->second->name;
  return res;
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
  return false;
}

object
LsysContext::getObject(const std::string& name, const boost::python::object& defaultvalue) const{
  return defaultvalue;
}

void 
LsysContext::setObject(const std::string& name, 
			   const boost::python::object& o){
}

void 
LsysContext::delObject(const std::string& name) {
}

bool 
LsysContext::copyObject(const std::string& name, LsysContext * sourceContext)
{
	if (!sourceContext) return false;
	PyObject * obj = PyDict_GetItemString(sourceContext->Namespace(),name.c_str());
	if (obj == NULL) return false;
	PyDict_SetItemString(Namespace(),name.c_str(),obj);
	return true;
}


void
LsysContext::updateNamespace(const boost::python::dict& d){
}

void
LsysContext::getNamespace(boost::python::dict& d) const{
}

void
LsysContext::clearNamespace() {
}

std::string 
LsysContext::str() const {
  std::stringstream s;
  s << "<LsysContext instance at " << this;
  if(__ignore_method)
	s << " ignoring " << __keyword.size() << " modules>";
  else
	s << " considering " << __keyword.size() << " modules>";
  return s.str();
}
void 
LsysContext::compile(const std::string& code) {
  ContextMaintainer c(this);
  Compilation::compile(code,Namespace(),Namespace());
}

object 
LsysContext::compile(const std::string& name, const std::string& code) {
  if(!code.empty()){
	  Compilation::compile(code,Namespace(),Namespace());
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
	  PyRun_String(code.c_str(),Py_eval_input,Namespace(),local_namespace.ptr())));
	return object(result);
  }
  return object();
}


object 
LsysContext::try_evaluate(const std::string& code) {
  if(!code.empty()){
	dict local_namespace;
	handle<> result(allow_null( 
	  PyRun_String(code.c_str(),Py_eval_input,Namespace(),local_namespace.ptr())));
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


void
LsysContext::start(){
  func("Start");
}

void 
LsysContext::end(){
  func("End");
}

void
LsysContext::end(AxialTree& lstring)
{ controlMethod("End",lstring); }

void
LsysContext::end(AxialTree& lstring, const PGL::ScenePtr& scene)
{ controlMethod("End",lstring,scene); }


void 
LsysContext::startEach(){
  func("StartEach");
}

void 
LsysContext::endEach(){
  func("EndEach");
}
void
LsysContext::endEach(AxialTree& lstring)
{ controlMethod("EndEach",lstring); }

void
LsysContext::endEach(AxialTree& lstring, const PGL::ScenePtr& scene)
{ controlMethod("EndEach",lstring,scene); }


void
LsysContext::controlMethod(const std::string& name, AxialTree& lstring){
  ContextMaintainer c(this);
  if (hasObject(name)){
	reference_existing_object::apply<AxialTree*>::type converter;
	PyObject* obj = converter( &lstring );
	object real_obj = object( handle<>( obj ) );
	getObject(name)(real_obj);
  }
}

void
LsysContext::controlMethod(const std::string& name, AxialTree& lstring, const PGL::ScenePtr& scene)
{
  ContextMaintainer c(this);
  if (hasObject(name)){
	reference_existing_object::apply<AxialTree*>::type converter;
	PyObject* obj = converter( &lstring );
	object real_obj = object( handle<>( obj ) );
	getObject(name)(real_obj, scene);
  }
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
LsysContext::func(const std::string& funcname){
  ContextMaintainer c(this);
  if (hasObject(funcname))getObject(funcname)();
}

void 
LsysContext::check_init_functions()
{
	if (hasObject("EndEach")) {
		try {
			__nbargs_of_endeach = extract<size_t>(getObject("EndEach").attr("func_code").attr("co_argcount"))();
		}
		catch (...) { PyErr_Clear(); __nbargs_of_endeach = 0; }
	}
	else __nbargs_of_endeach = 0;
	if (hasObject("End")) {
		try {
			__nbargs_of_end = extract<size_t>(getObject("End").attr("func_code").attr("co_argcount"))();
		}
		catch (...) { PyErr_Clear(); __nbargs_of_end = 0; }
	}
	else __nbargs_of_end = 0;
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
LsysContext::isSelectionRequired() const
{ return __selection_required; }

void 
LsysContext::setSelectionRequired(bool enabled)
{ 
	if (__selection_required != enabled){
		__selection_required = enabled; 
		options.setSelection("Selection Required",(size_t)enabled);
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

LocalContext::LocalContext(bool with_initialisation):
  LsysContext()
{ 
   if(with_initialisation)initialisation();
}

LocalContext::~LocalContext()
{
	// std::cerr << "del of local context" << std::endl;
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
					std::vector<LsysContext *>::iterator it2 = it;
					it2++;
					LSYSCONTEXT_STACK.erase(it);
					it = it2;
				}
			}
		}
	}
}

bool 
LocalContext::hasObject(const std::string& name) const{
  return __namespace.has_key(name);
}

object 
LocalContext::getObject(const std::string& name, const boost::python::object& defaultvalue) const{
  return __namespace.get(name,defaultvalue);
}

void 
LocalContext::setObject(const std::string& name, 
			   const boost::python::object& o){
  __namespace[name] = o;
}

void 
LocalContext::delObject(const std::string& name) {
  __namespace[name].del();
}

void
LocalContext::clearNamespace() {
  __namespace.clear();
  initialisation();
}

void 
LocalContext::updateNamespace(const dict& d){
  __namespace.update(d);
}

void 
LocalContext::getNamespace(dict& d) const{
  d.update(__namespace);
}

PyObject * 
LocalContext::Namespace() const 
{ return __namespace.ptr(); }

void 
LocalContext::initialisation()
{
   if(!DEFAULT_LSYSCONTEXT) createDefaultContext();
   defaultContext()->getNamespace(__namespace);
}

/*---------------------------------------------------------------------------*/

GlobalContext::GlobalContext():
  LsysContext(){
	// handle<> main_module(borrowed(  ));
    // __namespace = handle<>(borrowed( PyModule_GetDict(main_module.get())));
    __namespace = handle<>(borrowed( PyModule_GetDict(PyImport_AddModule("__main__"))));
}

GlobalContext::~GlobalContext()
{
	// std::cerr << "global context deleted" << std::endl;
	assert(LSYSCONTEXT_STACK.empty() && isCurrent());
	// assert(LSYSCONTEXT_STACK.empty());
}

bool 
GlobalContext::hasObject(const std::string& name) const{
#if PY_MAJOR_VERSION > 2 
  return PyDict_Contains(__namespace.get(),object(name).ptr());
#elif PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION > 3
  return PyDict_Contains(__namespace.get(),object(name).ptr());
#else
  try {
	object t = getObject(name);
	return t != object();
  }
  catch( error_already_set ){ PyErr_Clear(); return false; }
#endif
}

object 
GlobalContext::getObject(const std::string& name, const boost::python::object& defaultvalue) const{
  handle<> res(allow_null(PyDict_GetItemString(__namespace.get(),name.c_str())));
  if(res) {
	return object(handle<>(borrowed(res.get())));
  }
  else {
	// PyErr_SetString(PyExc_KeyError, name.c_str());
    // throw_error_already_set();
	return defaultvalue;
  }
}

void 
GlobalContext::setObject(const std::string& name, 
			   const boost::python::object& o){
  PyDict_SetItemString(__namespace.get(),name.c_str(),o.ptr());
}

void 
GlobalContext::delObject(const std::string& name) {
  PyDict_DelItemString(__namespace.get(),name.c_str());
}

void
GlobalContext::clearNamespace() {
  // PyDict_Clear(__namespace.get());
	__local_namespace.clear();
}

void 
GlobalContext::updateNamespace(const dict& d){
  PyDict_Update(__namespace.get(),d.ptr());
}

void 
GlobalContext::getNamespace(dict& d) const{
  PyDict_Merge(d.ptr(),__namespace.get(),true);
}


PyObject * 
GlobalContext::Namespace()  const 
{ return __namespace.get(); }



object 
GlobalContext::compile(const std::string& name, const std::string& code) {
  if(!code.empty()){
	Compilation::compile(code,__namespace.get(),__local_namespace.ptr());
	return __local_namespace[name];
  }
  return object();
}

boost::python::object GlobalContext::__reprFunc;

boost::python::object 
GlobalContext::getFunctionRepr() {
	if(__reprFunc == boost::python::object())
		__reprFunc = globalContext()->getObject("__builtins__").attr("__dict__")["repr"];
	return __reprFunc;
}


/*---------------------------------------------------------------------------*/

