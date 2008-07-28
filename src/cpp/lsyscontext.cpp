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
#include "lpy_parser.h"
#include "tracker.h"
#include <stack>
using namespace boost::python;
LPY_USING_NAMESPACE


/*---------------------------------------------------------------------------*/

const std::string LsysContext::InitialisationFunctionName("__initialiseContext__");
const std::string LsysContext::AxiomVariable("__axiom__");
const std::string LsysContext::DerivationLengthVariable("__derivation_length__");
const std::string LsysContext::DecompositionMaxDepthVariable("__decomposition_max_depth__");
const std::string LsysContext::HomomorphismMaxDepthVariable("__homomorphism_max_depth__");

double LsysContext::DefaultAnimationTimeStep(0.05);
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
static LsysContext * CURRENT_LSYSCONTEXT = LsysContext::globalContext();
// static LsysContext * CURRENT_LSYSCONTEXT = NULL;

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
LsysContext::globalContext()
{ 
    if(!GLOBAL_LSYSCONTEXT) GLOBAL_LSYSCONTEXT = new GlobalContext();
	return GLOBAL_LSYSCONTEXT; 
}

void createDefaultContext()
{ 
    if(!DEFAULT_LSYSCONTEXT){
        DEFAULT_LSYSCONTEXT = new LocalContext(false);
        // copy __builtins__ for import and all.
		DEFAULT_LSYSCONTEXT->copyObject("__builtins__",LsysContext::globalContext());
		DEFAULT_LSYSCONTEXT->copyObject("__doc__",LsysContext::globalContext());
        // import pylsystems
        DEFAULT_LSYSCONTEXT->execute("from openalea.lpy import *");
    }
}


LsysContext *
LsysContext::defaultContext()
{ 
    if(!DEFAULT_LSYSCONTEXT) createDefaultContext();
    return DEFAULT_LSYSCONTEXT; 
}

LsysContext *
LsysContext::currentContext()
{ 
    if(!CURRENT_LSYSCONTEXT) CURRENT_LSYSCONTEXT = defaultContext();
	return CURRENT_LSYSCONTEXT; 
}

void 
LsysContext::makeCurrent() 
{ 
  LsysContext * previous = currentContext();
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
{ return CURRENT_LSYSCONTEXT == this; }


void LsysContext::currentEvent()
{
	for(LsysOptions::iterator it = options.begin(); it != options.end(); ++it)
		(*it)->activateSelection();
	for(ModuleClassList::const_iterator it = __modules.begin(); it != __modules.end(); ++it)
		(*it)->activate();
}

void LsysContext::doneEvent()
{
	for(ModuleClassList::const_iterator it = __modules.begin(); it != __modules.end(); ++it)
		(*it)->desactivate();
		// ModuleClassTable::get().remove(*it);
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
__animation_step(DefaultAnimationTimeStep),
__iteration_nb(0)
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
  __animation_step(lsys.__animation_step),
  __iteration_nb(0)
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
  __animation_step =lsys.__animation_step;
  return *this;
}

LsysContext::~LsysContext()
{
	DecTracker(LsysContext)
	// std::cerr << "context deleted" << std::endl;
}

void LsysContext::init_options()
{
	/** selection required option */
	LsysOption* option = options.add("Selection Required","Set whether selection check in GUI is required or not. Selection is then transform in X module in the Lstring.","Interaction");
	option->addValue("Disabled",this,&LsysContext::setSelectionRequired,false,"Disable Selection Check.");
	option->addValue("Enabled",this,&LsysContext::setSelectionRequired,true,"Enable Selection Check.");
	option->setDefault(0);
	/** module matching option */
	option = options.add("Module matching","Specify the way module are match to rules pattern","Matching");
	option->addValue("Simple",&ParamModule::setMatchingMethod,ParamModule::eSimple,"Simple module matching : Same name and same number of arguments . '*' module allowed.");
	option->addValue("Extension : *args",&ParamModule::setMatchingMethod,ParamModule::eWithStar,"Add matching rule that * module can match any module and allow *args to match a number of module arguments");
	option->addValue("Extensions : *args and arg value constraints",&ParamModule::setMatchingMethod,ParamModule::eWithStarNValueConstraint,"With * module and *args. A module argument can also be set to a given value adding thus a new matching constraint.");
	option->setDefault(2);
	option->setGlobal(true);
	/** module declaration option */
	option = options.add("Module declaration","Specify if module declaration is mandatory.","Module");
	option->addValue("On the fly",&ModuleClassTable::setMandatoryDeclaration,false,"Module declaration is made on the fly when parsing string.");
	option->addValue("Mandatory",&ModuleClassTable::setMandatoryDeclaration,true,"Module declaration is mandatory before use in string.");
	option->setDefault(0);
	option->setGlobal(true);
}

void 
LsysContext::clear(){
  __keyword.clear();
  __ignore_method = true;
  __direction = eForward;
  __group = 0;
  __selection_required = false;
  __iteration_nb = 0;
  __modules.clear();
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

void 
LPY::consider(const std::string& modules)
{ LsysContext::currentContext()->consider(modules); }

void 
LPY::ignore(const std::string& modules)
{ LsysContext::currentContext()->ignore(modules); }

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
	std::vector<std::string> moduleclasses = parse_moddeclaration(modules);
	bool iscurrent = isCurrent();
	for(std::vector<std::string>::const_iterator it = moduleclasses.begin();
		it != moduleclasses.end(); ++it)
	{
		ModuleClassPtr mod = ModuleClassTable::get().declare(*it);
		__modules.push_back(mod);
		mod->activate(iscurrent);
	}
}

void 
LsysContext::declare(ModuleClassPtr module)
{
	__modules.push_back(module);
}

void LPY::declare(const std::string& modules)
{ LsysContext::currentContext()->declare(modules); }

/*---------------------------------------------------------------------------*/

bool 
LsysContext::hasObject(const std::string& name) const{
  return false;
}

object
LsysContext::getObject(const std::string& name) const{
  return object();
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
LsysContext::execute(const std::string& code) {
  ContextMaintainer c(this);
  if(!code.empty())
  handle<>( PyRun_String(code.c_str(),Py_file_input,Namespace(),Namespace()) );
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

object 
LsysContext::compile(const std::string& name, const std::string& code) {
  if(!code.empty()){
	// dict local_namespace;
	handle<>( PyRun_String(code.c_str(),Py_file_input,Namespace(),Namespace()) );
	// handle<>( PyRun_String(code.c_str(),Py_file_input,Namespace(),local_namespace.ptr()) );
	return getObject(name);
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
LsysContext::startEach(){
  func("StartEach");
}

void 
LsysContext::endEach(){
  func("EndEach");
}

void LsysContext::initialise()
{
  ContextMaintainer c(this);
  reference_existing_object::apply<LsysContext*>::type converter;
  PyObject* obj = converter( this );
  object real_obj = object( handle<>( obj ) );
  if (hasObject(InitialisationFunctionName))getObject(InitialisationFunctionName)(real_obj);
}

void 
LsysContext::setStart(object func){
  setObject("Start",func);
}

void 
LsysContext::setEnd(object func){
  setObject("Stop",func);
}

void 
LsysContext::setStartEach(object func){
  setObject("StartEach",func);
}

void 
LsysContext::setEndEach(object func){
  setObject("EndEach",func);
}

void 
LsysContext::func(const std::string& funcname){
  ContextMaintainer c(this);
  if (hasObject(funcname))getObject(funcname)();
}

/*---------------------------------------------------------------------------*/

void 
LsysContext::nproduce(const AxialTree& prod)
{
    __nproduction += prod;
}

void 
LsysContext::nproduce(const boost::python::list& prod)
{
    __nproduction += prod;
}

void LPY::nproduce(const AxialTree& prod)
{ LsysContext::currentContext()->nproduce(prod); }

void LPY::nproduce(const boost::python::list& prod)
{ LsysContext::currentContext()->nproduce(prod); }

void LPY::nproduce(const std::string& prod)
{ LsysContext::currentContext()->nproduce(AxialTree(prod)); }

void 
LsysContext::reset_nproduction()
{
    __nproduction.clear();
}

/*---------------------------------------------------------------------------*/

void LPY::useGroup(size_t gid)
{ LsysContext::currentContext()->useGroup(gid); }

size_t LPY::getGroup()
{ return LsysContext::currentContext()->getGroup(); }

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

size_t LPY::getIterationNb()
{ return LsysContext::currentContext()->getIterationNb(); }

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

void 
LPY::setSelectionRequired(bool enabled)
{ LsysContext::currentContext()->setSelectionRequired(enabled); }

bool 
LPY::isSelectionRequired()
{ return LsysContext::currentContext()->isSelectionRequired(); }

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
LocalContext::getObject(const std::string& name) const{
  return __namespace[name];
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
GlobalContext::getObject(const std::string& name) const{
  handle<> res(allow_null(PyDict_GetItemString(__namespace.get(),name.c_str())));
  if(res) {
	return object(handle<>(borrowed(res.get())));
  }
  else {
	PyErr_SetString(PyExc_KeyError, name.c_str());
    throw_error_already_set();
	return object();
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
	handle<>( PyRun_String(code.c_str(),Py_file_input,__namespace.get(),__local_namespace.ptr()) );
	return __local_namespace[name];
  }
  return object();
}

/*---------------------------------------------------------------------------*/

