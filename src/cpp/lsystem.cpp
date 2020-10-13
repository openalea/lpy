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

#include "lsystem.h"
#include "interpretation.h"
#include "plot.h"
#include "tracker.h"
#include <QtCore/QThread>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <plantgl/tool/sequencer.h>
#include "debug_tool.h"

using namespace boost::python;
TOOLS_USING_NAMESPACE
PGL_USING_NAMESPACE
LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

#ifdef MULTI_THREADED_LSYSTEM
#define ACQUIRE_RESSOURCE  LsysAcquirer ressource(this); 
#define RELEASE_RESSOURCE  

#define PRINT_RESSOURCE(msg) 
// printf(msg": %i : %i %i %i; %s %i\n",__LINE__,(int)__ressource,(int)this,(int)QThread::currentThread(),(isRunning()?"True":"False"),__ressource->count);
#else
#define ACQUIRE_RESSOURCE  
#define PRINT_RESSOURCE(msg) 
#define RELEASE_RESSOURCE
#endif

/*---------------------------------------------------------------------------*/

#ifdef MULTI_THREADED_LSYSTEM
Lsystem::LsysRessource::LsysRessource() : mutex(QMutex::NonRecursive) {}
Lsystem::LsysAcquirer::LsysAcquirer(const Lsystem * lsys) : __lsys(lsys) { lsys->acquire(); }
Lsystem::LsysAcquirer::~LsysAcquirer() { __lsys->release(); }
#endif

/*---------------------------------------------------------------------------*/

Lsystem::RuleGroup::RuleGroup():
  __prodhasquery(false),
  __dechasquery(false),
  __inthasquery(false)
{}

const RuleSet& 
Lsystem::RuleGroup::getGroup(eRuleType t) const
{
    switch (t){
        case eProduction:
            return production;
            break;
        case eDecomposition:
            return decomposition;
            break;
        case eInterpretation:
            return interpretation;
            break;
        default:
            return production;
            break;
    }
}

RuleSet& 
Lsystem::RuleGroup::getGroup(eRuleType t)
{
    switch (t){
        case eProduction:
            return production;
            break;
        case eDecomposition:
            return decomposition;
            break;
        case eInterpretation:
            return interpretation;
            break;
        default:
            return production;
            break;
    }
}

bool Lsystem::RuleGroup::hasQuery(eRuleType t) const
{
    switch (t){
        case eProduction:
            return __prodhasquery;
            break;
        case eDecomposition:
            return __dechasquery;
            break;
        case eInterpretation:
            return __inthasquery;
            break;
        default:
            return __prodhasquery;
            break;
    }
}


/*---------------------------------------------------------------------------*/

Lsystem::Lsystem():
__max_derivation(1),
__decomposition_max_depth(1),
__interpretation_max_depth(1),
__currentGroup(0),
__context(),
__newrules(false)
#ifdef MULTI_THREADED_LSYSTEM
,__ressource(new LsysRessource())
#endif
{
  IncTracker(Lsystem)
  PRINT_RESSOURCE("create")
}



Lsystem::Lsystem(const std::string& filename):
__max_derivation(1),
__decomposition_max_depth(1),
__interpretation_max_depth(1),
__context(),
__newrules(false)
#ifdef MULTI_THREADED_LSYSTEM
,__ressource(new LsysRessource())
#endif
{
  IncTracker(Lsystem)
  PRINT_RESSOURCE("create")
  read(filename);
}

Lsystem::Lsystem(const std::string& filename, 
			     const boost::python::dict& parameters):
__max_derivation(1),
__decomposition_max_depth(1),
__interpretation_max_depth(1),
__context(),
__newrules(false)
#ifdef MULTI_THREADED_LSYSTEM
,__ressource(new LsysRessource())
#endif
{
  IncTracker(Lsystem)
  PRINT_RESSOURCE("create")
  read(filename,parameters);
}

Lsystem::Lsystem(const Lsystem& lsys):
__rules(lsys.__rules),
__max_derivation(lsys.__max_derivation),
__decomposition_max_depth(lsys.__decomposition_max_depth),
__interpretation_max_depth(lsys.__interpretation_max_depth),
__context(lsys.__context),
__newrules(lsys.__newrules)
#ifdef MULTI_THREADED_LSYSTEM
,__ressource(new LsysRessource())
#endif
{
  IncTracker(Lsystem)
  PRINT_RESSOURCE("create")
}

Lsystem& Lsystem::operator=(const Lsystem& lsys)
{
    __rules = lsys.__rules;
    __max_derivation =lsys.__max_derivation;
    __decomposition_max_depth = lsys.__decomposition_max_depth;
    __interpretation_max_depth = lsys.__interpretation_max_depth;
    __context = lsys.__context;
    return *this;
}

Lsystem::RuleGroup& Lsystem::__group(size_t group)
{
    if (group >= __rules.size()){
        size_t s = __rules.size();
        for(;s <= group;++s)
            __rules.push_back(RuleGroup());
    }
    return __rules[group];
}

const Lsystem::RuleGroup& Lsystem::__group(size_t group) const
{
    return __rules[group];
}

Lsystem::~Lsystem()
{
 DecTracker(Lsystem)
 clear();
#ifdef MULTI_THREADED_LSYSTEM
 delete __ressource;
#endif
 if(isCurrent())done();
 PRINT_RESSOURCE("delete")
}

void 
Lsystem::clear(){
  ACQUIRE_RESSOURCE
  __clear();
  RELEASE_RESSOURCE
}
void 
Lsystem::__clear(){
  __axiom.clear();
  __rules.clear();
  __max_derivation = 1;
  __decomposition_max_depth = 1;
  __interpretation_max_depth = 1;
  __context.clear();
  reference_existing_object::apply<Lsystem*>::type converter;
  PyObject* obj = converter( this );
  object real_obj = object( handle<>( obj ) );
  __context.setObject("__lsystem__",real_obj);

}



std::string 
Lsystem::str() const {
  ACQUIRE_RESSOURCE
  std::stringstream s;
  // s << "Lsystem:\n";
  s << "Axiom: " << __axiom.str() << '\n';

  if(!__context.__modules.empty()){
	s << "module ";
	for(ModuleClassList::const_iterator it = __context.__modules.begin();
		it != __context.__modules.end(); ++it){
		if(it != __context.__modules.begin()) s << ',';
		s << (*it)->name; 
	}
	s << '\n';
  }

  s << "derivation length: " << __max_derivation << '\n';
  s << "production:\n";
  size_t gid = 0;
  for (RuleGroupList::const_iterator g = __rules.begin(); g != __rules.end(); ++g){
          if (gid != 0){
            s << "group: " << gid << std::endl;
            s << "production:\n";
          }
          for (RuleSet::const_iterator   i = g->production.begin();
              i != g->production.end(); i++){
                  s << i->str() << '\n';
          }
          if(!g->decomposition.empty()){
              s << "decomposition:\n";
              if (gid == 0)
                s << "maximum depth:"  << __decomposition_max_depth << '\n';
              for (RuleSet::const_iterator i = g->decomposition.begin();
                  i != g->decomposition.end(); i++){
                      s << i->str()+'\n';
              }
          }
          if(!g->interpretation.empty()){
              s << "interpretation:\n";
              if (gid == 0)
                s << "maximum depth:"  << __interpretation_max_depth << '\n';
              for (RuleSet::const_iterator i = g->interpretation.begin();
                  i != g->interpretation.end(); i++){
                      s << i->str()+'\n';
              }
          }
          ++gid;
  }
  s << "endlsystem";
  return s.str();
  RELEASE_RESSOURCE
}

std::string 
Lsystem::code()  {
  ACQUIRE_RESSOURCE
  std::stringstream s;
  s << "Lsystem:\n";
  s << "Axiom: " << __axiom.str() << '\n';
  s << "derivation length: " << __max_derivation << '\n';
  s << "production:\n";
  size_t gid = 0;
  for (Lsystem::RuleGroupList::iterator g = __rules.begin(); g != __rules.end(); ++g){
          if (gid != 0){
            s << "group: " << gid << std::endl;
            s << "production:\n";
          }
          for (RuleSet::iterator   i = g->production.begin(); i != g->production.end(); ++i){
                  s << i->getCode() << '\n';
          }
          if(!g->decomposition.empty()){
              s << "decomposition:\n";
              if (gid == 0)
                s << "maximum depth:"  << __decomposition_max_depth << '\n';
              for (RuleSet::iterator i = g->decomposition.begin(); i != g->decomposition.end(); ++i){
                      s << i->getCode()+'\n';
              }
          }
          if(!g->interpretation.empty()){
              s << "interpretation:\n";
              if (gid == 0)
                s << "maximum depth:"  << __interpretation_max_depth << '\n';
              for (RuleSet::iterator i = g->interpretation.begin(); i != g->interpretation.end(); ++i){
                      s << i->getCode()+'\n';
              }
          }
          ++gid;
  }
  s << "endlsystem";
  return s.str();
  RELEASE_RESSOURCE
}

bool 
Lsystem::isCompiled(){
  ACQUIRE_RESSOURCE
  RuleSet::const_iterator i;
  for (RuleGroupList::const_iterator g = __rules.begin(); g != __rules.end(); ++g)
  {
      for ( i = g->production.begin();    i != g->production.end(); ++i)
          if(!i->isCompiled())return false;
      for ( i = g->decomposition.begin(); i != g->decomposition.end(); ++i)
          if(!i->isCompiled())return false;
      for ( i = g->interpretation.begin();  i != g->interpretation.end(); ++i)
          if(!i->isCompiled())return false;
  }
  return true;
  RELEASE_RESSOURCE
}

void 
Lsystem::compile(){
  ACQUIRE_RESSOURCE
  ContextMaintainer m(&__context);
  RuleSet::iterator i;
  for (RuleGroupList::iterator g = __rules.begin();
      g != __rules.end(); ++g)
  {
      for (i = g->production.begin();    i != g->production.end(); ++i)
          i->compile();
      for (i = g->decomposition.begin(); i != g->decomposition.end(); ++i)
          i->compile();
      for (i = g->interpretation.begin();  i != g->interpretation.end(); ++i)
          i->compile();
  }
  RELEASE_RESSOURCE
}

void 
Lsystem::__importPyFunctions(){
  ContextMaintainer m(&__context);
  RuleSet::iterator i;
  for (RuleGroupList::iterator g = __rules.begin(); g != __rules.end(); ++g)
  {
      for ( i = g->production.begin();    i != g->production.end(); ++i)
          i->importPyFunction();
      for ( i = g->decomposition.begin(); i != g->decomposition.end(); ++i)
          i->importPyFunction();
      for ( i = g->interpretation.begin();  i != g->interpretation.end(); ++i)
          i->importPyFunction();
  }
}

#include <fstream>

void 
Lsystem::read(const std::string& filename, 
			  const boost::python::dict& parameters){
  clear();
  std::ifstream file(filename.c_str());
  if(file){
    setFilename(filename);

/* This part raise seg fault while trying to read certain lpy files.
 * It is replaced by the usage of file.rdbuf here below. TC. nov 2009
 *
 * std::string content;
#define bufsize 100000
	char text[bufsize+1];
	text[bufsize] = '\0';
	while(!file.eof()){
		file.read(text,bufsize);
	  content += std::string(text);
	}
	set(content);
*/

  std::stringstream buffer; 
  buffer << file.rdbuf();
  file.close();
  //std::cout << "buffer : " << buffer.str() << '\n';
  //std::cout << "Taille du buffer : " << buffer.str().size() << '\n';
  set(buffer.str(),NULL,parameters);
  }
  else {
	LsysError('\''+filename+"': No such file or directory.");
  }
}

void Lsystem::setFilename( const std::string& filename )
{
    __context.setObject("__file__",boost::python::object(filename));
    std::string name =  QFileInfo(filename.c_str()).baseName().toStdString();
    __context.setObject("__name__",boost::python::object(name));
	// std::string path =  QFileInfo(filename.c_str()).absoluteDir().path().toStdString();
    // __context.setObject("__path__",boost::python::object(path));
}

std::string Lsystem::getFilename( ) const
{
    if(__context.hasObject("__file__"))
        return extract<std::string>(__context.getObject("__file__"));
    else return std::string();
}

std::string Lsystem::getShortFilename( ) const
{
    std::string filename = getFilename();
    if (filename.empty())return filename;
    else return QFileInfo(filename.c_str()).fileName().toStdString();
}

LsysRule& 
Lsystem::__addProductionRule( const std::string& code, size_t groupid, int lineno, const ConsiderFilterPtr filter ){
  RuleGroup& group = __group(groupid);
  LsysRule r(group.production.size(),groupid,'p',lineno);
  r.set(code);
  r.consider(filter);
  group.production.push_back(r);
  if (r.hasQuery())group.__prodhasquery = true;
  __newrules = true;
  return *(group.production.end()-1);
}

LsysRule& 
Lsystem::__addDecompositionRule( const std::string& code, size_t groupid , int lineno, const ConsiderFilterPtr filter ){
  RuleGroup& group = __group(groupid);
  LsysRule r(group.decomposition.size(),groupid,'d',lineno);
  r.set(code);
  r.consider(filter);
  group.decomposition.push_back(r);
  if (r.hasQuery())group.__dechasquery = true;
  __newrules = true;
  return *(group.decomposition.end()-1);
}

LsysRule&
Lsystem::__addInterpretationRule( const std::string& code, size_t groupid, int lineno, const ConsiderFilterPtr filter ){
  RuleGroup& group = __group(groupid);
  LsysRule r(group.interpretation.size(),groupid,'h',lineno);
  r.set(code);
  r.consider(filter);
  if (!r.isContextFree())LsysWarning("Interpretation rules should be context free. Contexts not supported for multiple iterations.");
  group.interpretation.push_back(r);
  if (r.hasQuery())group.__inthasquery = true;
  __newrules = true;
  return *(group.interpretation.end()-1);
}

LsysRule&
Lsystem::__addRule( const std::string& rule, int type, size_t group, int lineno, const ConsiderFilterPtr filter ){
  switch(type){
  case 1:
	return __addDecompositionRule(rule,group,lineno,filter);
	break;
  case 2:
	return __addInterpretationRule(rule,group,lineno,filter);
	break;
  default:
	return __addProductionRule(rule,group,lineno,filter);
	break;
  } 
}

void 
Lsystem::addProductionRule( const std::string& code, size_t group, const ConsiderFilterPtr filter ){
  // ACQUIRE_RESSOURCE
  ContextMaintainer m(&__context);
  LsysRule& r = __addProductionRule(code,group,-1,filter);
  r.compile();
  // RELEASE_RESSOURCE
}

void 
Lsystem::addDecompositionRule( const std::string& code, size_t group, const ConsiderFilterPtr filter ){
  // ACQUIRE_RESSOURCE
  ContextMaintainer m(&__context);
  LsysRule& r = __addDecompositionRule(code,group,-1,filter);
  r.compile();
  // RELEASE_RESSOURCE
}

void 
Lsystem::addInterpretationRule( const std::string& code, size_t group, const ConsiderFilterPtr filter ){
  // ACQUIRE_RESSOURCE
  ContextMaintainer m(&__context);
  LsysRule& r = __addInterpretationRule(code,group,-1,filter);
  r.compile();
  // RELEASE_RESSOURCE
}

void 
Lsystem::addRule(  const LsysRule& rule, int type, size_t groupid){
  switch(type){
  case 1:
    __group(groupid).decomposition.push_back(rule);
    if (rule.hasQuery())__group(groupid).__dechasquery = true;
    break;
  case 2:
    if (!rule.isContextFree())LsysWarning("Interpretation rules should be context free. Contexts not supported for multiple iterations.");
    __group(groupid).interpretation.push_back(rule);
    if (rule.hasQuery())__group(groupid).__inthasquery = true;
	break;
  default:
    __group(groupid).production.push_back(rule);
    if (rule.hasQuery())__group(groupid).__prodhasquery = true;
	break;
  }
  __newrules = true;
}

void Lsystem::addRule( const std::string& rule, int type, size_t group, const ConsiderFilterPtr filter ){
	__newrules = true;
    ContextMaintainer m(&__context);
    LsysRule& r = __addRule(rule,type,group,-1,filter);
	r.compile();
}

bool 
Lsystem::empty( ) const {
  return __rules.empty();
}

size_t 
Lsystem::nbProductionRules( size_t group ) const {
  if (__rules.size() < group) return 0;
  return __group(group).production.size();
}

size_t 
Lsystem::nbDecompositionRules( size_t group ) const {
  if (__rules.size() < group) return 0;
  return __group(group).decomposition.size();
}

size_t Lsystem::nbInterpretationRules( size_t group ) const {
  if (__rules.size() < group) return 0;
  return __group(group).interpretation.size();
}

size_t Lsystem::nbTotalRules(  ) const {
  size_t nbrules = 0;
  for(RuleGroupList::const_iterator it = __rules.begin(); it != __rules.end(); ++it)
        nbrules += it->production.size()+it->decomposition.size()+it->interpretation.size();
  return nbrules;
}

size_t Lsystem::nbGroups( ) const {
  return __rules.size();
}

void Lsystem::addSubLsystem(const std::string& lfile)
{
	addSubLsystem(Lsystem(lfile));
}

void Lsystem::addSubLsystem(const Lsystem& sublsystem)
{
	printf("Add info from sublsystem '%s'\n",sublsystem.getFilename().c_str());
	context()->importContext(*sublsystem.context());
	size_t groupid = 0;
	for(std::vector<RuleGroup>::const_iterator itg = sublsystem.__rules.begin(); itg != sublsystem.__rules.end(); ++itg, ++groupid)
	{
		RuleGroup& rg = __group(groupid);
		rg.production.insert(rg.production.end(), itg->production.begin(),itg->production.end());
		rg.decomposition.insert(rg.decomposition.end(),itg->decomposition.begin(),itg->decomposition.end());
		rg.interpretation.insert(rg.interpretation.end(), itg->interpretation.begin(),itg->interpretation.end());
		rg.__prodhasquery = rg.__prodhasquery & itg->__prodhasquery;
		rg.__dechasquery = rg.__dechasquery & itg->__dechasquery;
		rg.__inthasquery = rg.__inthasquery & itg->__inthasquery;
	}

}


void 
Lsystem::setAxiom( const AxialTree& axiom ){
  ACQUIRE_RESSOURCE
  __axiom = axiom;
  RELEASE_RESSOURCE
}


const AxialTree& 
Lsystem::getAxiom( ) const {
  return __axiom;
}

pgl_hash_map_string<std::string> Lsystem::get_rule_fonction_table() const
{
  pgl_hash_map_string<std::string> result;
  for(RuleGroupList::const_iterator it = __rules.begin(); it != __rules.end(); ++it)
	{
		for(RuleSet::const_iterator itr = it->production.begin(); itr != it->production.end(); ++itr)
		{ result[itr->functionName()] = itr->name(); }
		for(RuleSet::const_iterator itr = it->decomposition.begin(); itr != it->decomposition.end(); ++itr)
		{ result[itr->functionName()] = itr->name(); }
		for(RuleSet::const_iterator itr = it->interpretation.begin(); itr != it->interpretation.end(); ++itr)
		{ result[itr->functionName()] = itr->name(); }
	}
  return result;
}

RulePtrMap Lsystem::__getRules(eRuleType type, size_t groupid, eDirection direction, bool * hasQuery)
{
    if(hasQuery)*hasQuery = false;
 	size_t nbgroups = __rules.size();
    if (groupid >= nbgroups) {
		if (nbgroups == 0) return RulePtrMap();
		else return __getRules(type,0,direction,hasQuery);
	}
    RulePtrSet result;
    const RuleSet& rules = __group(groupid).getGroup(type);
    for(RuleSet::const_iterator itr = rules.begin(); itr != rules.end(); ++itr)
        if(itr->isCompatible(direction)){
            result.push_back(&(*itr));
            if(hasQuery && itr->hasQuery())*hasQuery = true;
        }
    if (groupid > 0)
    {
        const RuleSet& rules = __group(0).getGroup(type);
        for(RuleSet::const_iterator itr = rules.begin(); itr != rules.end(); ++itr)
            if(itr->isCompatible(direction)){
                result.push_back(&(*itr));
                if(hasQuery && itr->hasQuery())*hasQuery = true;
            }
    }
    return RulePtrMap(result,direction);
}

AxialTree Lsystem::__debugStep(AxialTree& workingstring,
						  const RulePtrMap& ruleset,
						  bool query,
						  bool& matching,
						  eDirection direction,
						  Debugger& debugger){
  ContextMaintainer c(&__context);
  matching = false;
  if( workingstring.empty()) return workingstring;
  AxialTree targetstring;
  targetstring.reserve(workingstring.size());
  if ( query )__turtle_interpretation(workingstring,__context.envturtle);
  debugger.begin(workingstring,direction);
  if ( direction == eForward){
      AxialTree::const_iterator _it = workingstring.begin();
      AxialTree::const_iterator _it3 = _it;
      AxialTree::const_iterator _endit = workingstring.end();
      AxialTree::IteratorMap itermap; 

      while ( _it != _endit ) {
          if ( _it->isCut() )
              _it = workingstring.endBracket(_it);
          else{
              int match = 0;
			  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
              for(RulePtrSet::const_iterator _it2 = mruleset.begin();
                  _it2 != mruleset.end(); _it2++){
					  ArgList args;
					  size_t prodlength;
                      if((*_it2)->match(workingstring,_it,targetstring,_it3,args,&itermap)){
						  try {
							match = (*_it2)->applyTo(targetstring,args,&prodlength);
						  }catch(error_already_set){
							  if(!debugger.error_match(_it,_it3,targetstring,*_it2,args)){
								boost::python::throw_error_already_set();
							  }
							  else { PyErr_Clear(); }
							  match = -1;
						  }
						  if(match == 1) { 
							  if(debugger.shouldStop(_it,_it3,*_it2))debugger.total_match(_it,_it3,targetstring,prodlength,*_it2,args);
							  _it = _it3; break; 
						  }
						  else if(match == 0 && debugger.shouldStop(_it,_it3,*_it2))debugger.partial_match(_it,_it3,targetstring,*_it2,args);
                      }
              }
              if (match != 1){
                 targetstring.push_back(_it);
				 if(debugger.shouldStop(_it)) debugger.identity(_it,targetstring);
				 ++_it;
              }
              else matching = true;
          }
      }
  }
  else {
      AxialTree::const_iterator _it = workingstring.end()-1;
      AxialTree::const_iterator _it3 = _it;
      AxialTree::const_iterator _lastit = workingstring.begin();
      AxialTree::const_iterator _beg = workingstring.begin();
      AxialTree::const_iterator _end = workingstring.end();
      AxialTree::IteratorMap itermap; 
      while ( _it !=  _end) {
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end();  _it2++){
				  ArgList args;
				  size_t prodlength;
                  if((*_it2)->reverse_match(workingstring,_it,targetstring,_it3,args,&itermap)){
					  try {
						match = (*_it2)->reverseApplyTo(targetstring,args,&prodlength);
					  }catch(error_already_set){
						if(!debugger.error_match(_it3==_end?_beg:_it3+1,_it+1,targetstring,*_it2,args))
								boost::python::throw_error_already_set();
						else { PyErr_Clear(); match = false; }
					  }
                      if(match) { 							  
						  if(debugger.shouldStop(_it3==_end?_beg:_it3+1,_it+1,*_it2))debugger.total_match(_it3==_end?_beg:_it3+1,_it+1,targetstring,prodlength,*_it2,args);
						  _it = _it3; break; 
					  }
 					  else if(debugger.shouldStop(_it3==_end?_beg:_it3+1,_it+1,*_it2))debugger.partial_match(_it3==_end?_beg:_it3+1,_it+1,targetstring,*_it2,args);
                 }
          }
          if (!match){
              targetstring.push_front(_it);
			  if(debugger.shouldStop(_it))debugger.identity(_it,targetstring);
              if(_it != _lastit) --_it;
			  else _it = _end;
          }
          else matching = true;
      } ;
  }
  debugger.end(targetstring);
  return targetstring;
}

#include "axialtree_manip.h"

AxialTree 
Lsystem::__step(AxialTree& workingstring,
				const RulePtrMap& ruleset,
				bool query,
				bool& matching,
                eDirection direction){
  if (__context.multicoreProcessing()) { return __parallelStep(workingstring, ruleset, query, matching, direction);  }
  ContextMaintainer c(&__context);
  matching = false;
  if( workingstring.empty()) return workingstring;
  AxialTree targetstring;
  targetstring.reserve(workingstring.size());
  if ( query )__turtle_interpretation(workingstring,__context.envturtle);
  if ( direction == eForward){
      AxialTree::const_iterator _it = workingstring.begin();
      AxialTree::const_iterator _it3 = _it;
      AxialTree::const_iterator _endit = workingstring.end();
      AxialTree::IteratorMap * itermap = NULL;
      AxialTree::IteratorMap _itermap;
      if (__context.brackectMappingOptimLevel() >= 1) { itermap = &_itermap; }
      if (__context.brackectMappingOptimLevel() == 2) { endBracket(_it, _endit, itermap); }

      while ( _it != _endit ) {
          if ( _it->isCut() )
              _it = workingstring.endBracket(_it);
          else{
              bool match = false;
			  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
              for(RulePtrSet::const_iterator _it2 = mruleset.begin();
                  _it2 != mruleset.end(); _it2++){
					  ArgList args;
                      if((*_it2)->match(workingstring,_it,targetstring,_it3,args,itermap)){
                          match = (*_it2)->applyTo(targetstring,args);
						  if(match) { _it = _it3; break; }
                      }
              }
              if (!match){
                 targetstring.push_back(_it);++_it;
              }
              else matching = true;
          }
      }
  }
  else {
      AxialTree::const_iterator _it = workingstring.end()-1;
      AxialTree::const_iterator _it3 = _it;
      AxialTree::const_iterator _lastit = workingstring.begin();
      AxialTree::const_iterator _beg = workingstring.begin();
      AxialTree::const_iterator _end = workingstring.end();
      AxialTree::IteratorMap * itermap = NULL;
      AxialTree::IteratorMap _itermap;
      if (__context.brackectMappingOptimLevel() >= 1) { itermap = &_itermap; }
      if (__context.brackectMappingOptimLevel() == 2) { endBracket(_beg, _end, itermap); }

      while ( _it !=  _end) {
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end();  _it2++){
				  ArgList args;
                  if((*_it2)->reverse_match(workingstring,_it,targetstring,_it3,args,itermap)){
                      match = (*_it2)->reverseApplyTo(targetstring,args);
                      if(match) { _it = _it3; break; }
                  }
          }
          if (!match){
              targetstring.push_front(_it);
              if(_it != _lastit) --_it;
			  else _it = _end;
          }
          else matching = true;
      }
  }
  return targetstring;
}

#include <QtGlobal>
#if QT_VERSION >= 0x050000 
    #include <QtConcurrent/QtConcurrentRun>
    #include <QtConcurrent/QtConcurrentMap>
#else
    #include <QtCore/QtConcurrentRun>
    #include <QtCore/QtConcurrentMap>
#endif



AxialTree partialForwardStep(size_t beg, 
                             size_t size,
                             AxialTree& workingstring,
                             const RulePtrMap& ruleset)
{
      AxialTree targetstring;
      targetstring.reserve(size);

      AxialTree::const_iterator itbeg = workingstring.const_begin() + beg;
      AxialTree::const_iterator itend = itbeg;
      if (workingstring.size() >= beg + size) itend += size;
      else itend = workingstring.const_end();

      AxialTree::const_iterator _it = itbeg;
      AxialTree::const_iterator _it3 = _it;
      AxialTree::IteratorMap itermap; 


      while ( _it != itend ) {
          // printf("process module %lu %lu\n", beg, std::distance(workingstring.const_begin(), _it));
              bool match = false;
              const RulePtrSet& mruleset = ruleset[_it->getClassId()];
              for(RulePtrSet::const_iterator _it2 = mruleset.begin();
                  _it2 != mruleset.end(); _it2++){
                      ArgList args;
                      if((*_it2)->match(workingstring,_it,targetstring,_it3,args,&itermap)){
                          match = (*_it2)->applyTo(targetstring,args);
                          if(match) { _it = _it3; break; }
                      }
              }
              if (!match){
                 targetstring.push_back(_it); ++_it;
              }
      }
      return targetstring;

}

AxialTree partialBackwardStep(size_t beg, 
                              size_t size,
                              AxialTree& workingstring,
                              const RulePtrMap& ruleset)
{
      AxialTree targetstring;
      targetstring.reserve(size);

      AxialTree::const_iterator itbeg = workingstring.const_begin() + beg;
      AxialTree::const_iterator itend = itbeg;
      if (workingstring.size() >= beg + size) itend += size;
      else itend = workingstring.const_end();

      AxialTree::const_iterator _it = itend -1;
      AxialTree::const_iterator _lastit = itbeg;
      AxialTree::const_iterator _it3 = _it;
      AxialTree::IteratorMap itermap; 


      while ( _it != itend ) {
          // printf("process module %lu %lu\n", beg, std::distance(workingstring.const_begin(), _it));
              bool match = false;
              const RulePtrSet& mruleset = ruleset[_it->getClassId()];
              for(RulePtrSet::const_iterator _it2 = mruleset.begin();
                  _it2 != mruleset.end(); _it2++){
                      ArgList args;
                      if((*_it2)->reverse_match(workingstring,_it,targetstring,_it3,args,&itermap)){
                          match = (*_it2)->reverseApplyTo(targetstring,args);
                          if(match) { _it = _it3; break; }
                      }
              }
              if (!match){
                  targetstring.push_front(_it);
                  if(_it != _lastit) --_it;
                  else _it = itend;
              }
      }
      return targetstring;

}



void assemble(AxialTree& result, const AxialTree& second)
{  
    result += second; 
}

AxialTree Lsystem::partial_derivation(AxialTree& workingstring,
                               size_t beg, 
                               size_t size)
{
  eDirection ndir = getDirection();
  size_t group = __context.getGroup();
  bool productionHasQuery;
  bool decompositionHasQuery;
  RulePtrMap production = __getRules(eProduction,group,ndir,&productionHasQuery);
  RulePtrMap decomposition = __getRules(eDecomposition,group,ndir,&decompositionHasQuery);
  AxialTree partialresult;
  if (!production.empty()){
    if (ndir == eForward)
        partialresult = partialForwardStep(beg, size, workingstring, production);
    else
        partialresult = partialBackwardStep(beg, size, workingstring, production);
  }
  else {
    partialresult = AxialTree(workingstring.begin()+beg,workingstring.begin()+beg+size);    
  }
  if(!decomposition.empty()){
    bool decmatching;
    partialresult = __recursiveSteps(partialresult, decomposition, __decomposition_max_depth, decmatching);
  }
  return partialresult;
}


AxialTree 
Lsystem::__parallelStep(AxialTree& workingstring,
                        const RulePtrMap& ruleset,
                        bool query,
                        bool& matching,
                        eDirection direction){
  ContextMaintainer c(&__context);
  matching = false;
  if( workingstring.empty()) return workingstring;

  // check no pseudo lsystem

  // process cut symbol

  if ( query ) __turtle_interpretation(workingstring,__context.envturtle);
  
  int maxnbthread = QThreadPool::globalInstance()->maxThreadCount()-1;
  size_t lstringsize = workingstring.size();
  int nbsymbolperthread = lstringsize / maxnbthread;
  printf("Thread nb:%i elements:%i lstring size:%lu\n", maxnbthread, nbsymbolperthread, lstringsize);

  if (nbsymbolperthread * maxnbthread < lstringsize) ++nbsymbolperthread;


  if ( direction == eForward){

    QList<int> startmoduleid;
    for(int threadid = 0; threadid < maxnbthread; ++threadid) 
        startmoduleid.push_back(threadid * nbsymbolperthread);        

    QFuture<AxialTree> result = QtConcurrent::mappedReduced(startmoduleid, 
        boost::bind(partialForwardStep, _1, nbsymbolperthread, workingstring, ruleset),
        assemble, QtConcurrent::OrderedReduce|QtConcurrent::SequentialReduce);

    result.waitForFinished();
    return result.result();
  }
  else {
    QList<int> startmoduleid;
    for(int threadid = 0; threadid < maxnbthread; ++threadid) 
        startmoduleid.push_back(threadid * nbsymbolperthread);        

    QFuture<AxialTree> result = QtConcurrent::mappedReduced(startmoduleid, 
        boost::bind(partialBackwardStep, _1, nbsymbolperthread, workingstring, ruleset),
        assemble, QtConcurrent::OrderedReduce|QtConcurrent::SequentialReduce);

    result.waitForFinished();
    return result.result();
  }
}


AxialTree 
Lsystem::__stepWithMatching(AxialTree& workingstring,
				const RulePtrMap& ruleset,
				bool query,
                StringMatching& matching)
{
  ContextMaintainer c(&__context);
  if( workingstring.empty()) return workingstring;
  AxialTree targetstring;
  targetstring.reserve(workingstring.size());
  if ( query )LPY::turtle_interpretation(workingstring,__context.turtle);
  AxialTree::const_iterator _it = workingstring.begin();
  AxialTree::const_iterator _it3 = _it;
  AxialTree::const_iterator _endit = workingstring.end();
  AxialTree::IteratorMap itermap; 
  size_t prodlength;
  matching.clear();
  while ( _it != _endit ) {
      if ( _it->isCut() )
          _it = workingstring.endBracket(_it);
      else{
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end();  _it2++){
				  ArgList args;
                  if((*_it2)->match(workingstring,_it,targetstring,_it3,args,&itermap)){
                      match = (*_it2)->applyTo(targetstring,args,&prodlength);
					  if (match){
						matching.append(distance(_it,_it3),prodlength);
						_it = _it3;
						break;
					  }
                  }
          }
          if (!match){              
              targetstring.push_back(_it);++_it;
              matching.addIdentity(1);
          }
      }
  }
  return targetstring;
}

AxialTree 
Lsystem::__recursiveSteps(AxialTree& workingstring,
				          const RulePtrMap& ruleset, 
                          size_t maxdepth, bool& matching)
{
  ContextMaintainer c(&__context);
  matching = false;
  if( workingstring.empty()) return workingstring;
  AxialTree::const_iterator _it = workingstring.begin();
  AxialTree::const_iterator _it3 = _it;
  AxialTree::const_iterator _endit = workingstring.end();
  AxialTree targetstring;
  targetstring.reserve(workingstring.size());
  while ( _it != workingstring.end() ) {
      if ( _it->isCut() )
          _it = workingstring.endBracket(_it);
      else{
          AxialTree ltargetstring;
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end(); _it2++){
				ArgList args;
                if((*_it2)->match(workingstring,_it,ltargetstring,_it3,args)){
                      match = (*_it2)->applyTo(ltargetstring,args);
					  if(match) { _it = _it3; matching = true; break; }
                  }
          }
          if (match){
              if(maxdepth >1) {
                  bool submatch;
                  targetstring += __recursiveSteps(ltargetstring,ruleset,maxdepth-1, submatch);
                  if (submatch) matching = submatch;
              }
              else targetstring += ltargetstring;
          }
          else { targetstring.push_back(_it);++_it; }
      }
  }
  return targetstring;
}

template<class Interpreter>
void Lsystem::__gRecursiveInterpretation(AxialTree& workingstring,
										 const RulePtrMap& ruleset,
										 Interpreter& interpreter,
										 size_t maxdepth,
										 bool withid)
{
  ContextMaintainer c(&__context);
  if( workingstring.empty()) return ;
  AxialTree::iterator _itn = workingstring.begin();

  AxialTree::const_iterator _it = workingstring.begin();
  AxialTree::const_iterator _it3 = _it;
  AxialTree::const_iterator _endit = workingstring.end();
  size_t dist = 0;
  if (withid)  {
      AxialTree initturtle = interpreter.init();
      for(AxialTree::iterator _itl = initturtle.begin(); _itl != initturtle.end(); ++_itl)
            interpreter.interpret(_itl);  
      interpreter.start();
  }
  while ( _it != _endit && !interpreter.earlyReturn() ) {
      if ( _it->isCut() ){
	      _it3 = _it;
          _it = workingstring.endBracket(_it3);
          dist = distance(_it3,_it);
          _itn += dist;
          if(withid)interpreter.incId(dist);
      }
      else{
          AxialTree ltargetstring;
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end(); _it2++){
				  ArgList args;
                  if((*_it2)->match(workingstring,_it,ltargetstring,_it3,args)){
                      interpreter.parameters(args);
                      match = (*_it2)->applyTo(ltargetstring,args);
					  if (match) {
						dist = distance(_it,_it3);
						_it = _it3;
						_itn += dist;
						break;
					  }
                  }
          }
          if (match){
              if(maxdepth > 1) __gRecursiveInterpretation<Interpreter>(ltargetstring,ruleset,interpreter,maxdepth-1,false);
              else { 
                 for(AxialTree::iterator _itl = ltargetstring.begin();
					 _itl != ltargetstring.end(); ++_itl){
					 interpreter.interpret(_itl);
                 } 
              }
              if(withid)interpreter.incId(dist); 
          }
          else { 
			  interpreter.interpret(_itn);
			  if(withid) interpreter.incId();
			  ++_it; ++_itn;
          }
      }
  }
  if (withid)  {
      AxialTree finishturtle = interpreter.finalize();
      for(AxialTree::iterator _itl = finishturtle.begin(); _itl != finishturtle.end(); ++_itl)
            interpreter.interpret(_itl);  
      interpreter.stop();
  }
}

	

	struct TurtleInterpreter {
		TurtleInterpreter(Turtle& t, boost::python::object& pyt, LsysContext& c) : turtle(t), pyturtle(pyt), context(c)  {}
		Turtle& turtle;
        boost::python::object pyturtle;
        LsysContext& context;

		static inline bool earlyReturn() { return false; }

        inline AxialTree init() 
        { 
          turtle.start(); 
          turtle.setNoId(); 
          return context.startInterpretation(pyturtle);
        }

        inline AxialTree finalize() 
        { 
          turtle.setNoId(); 
          return context.endInterpretation(pyturtle);
        }
        
        
        inline void start() 
        { turtle.setId(0); }
        
		inline void stop()  
		{ turtle.stop();
		  if (!turtle.emptyStack()){
			printf("Turtle stack size : %lu\n",turtle.getStack().size());
			LsysError("Ill-formed string in interpretation: unmatched brackets");
			}
		}

		inline void interpret(AxialTree::iterator m){
			  m->interpret(turtle);
		}
		inline void incId(size_t nb = 1){
              turtle.incId(nb); 
		}

        inline void parameters(ArgList& args) {
            if (context.turtle_in_interpretation)
                args.push_front(pyturtle);
        }
	};

void 
Lsystem::__recursiveInterpretation(AxialTree& workingstring,
				                const RulePtrMap& ruleset,
                                Turtle& t,
                                boost::python::object& pyturtle,
                                size_t maxdepth)
{


	TurtleInterpreter i (t, pyturtle,__context);
	__gRecursiveInterpretation<TurtleInterpreter>(workingstring,ruleset,i,maxdepth);
}

	struct TurtleStepInterpreter {
		TurtleStepInterpreter(PglTurtle& t, boost::python::object& pyt, LsysContext& c) : turtle(t), pyturtle(pyt), context(c), timer(c.get_animation_timestep()) {}

		PglTurtle& turtle;
        boost::python::object pyturtle;
		LsysContext& context;
		PGL(Sequencer) timer;

		inline bool earlyReturn() { return context.isEarlyReturnEnabled(); }

        inline AxialTree init() 
        { turtle.start(); turtle.setNoId(); return context.startInterpretation(pyturtle); }

        inline AxialTree finalize() 
        { turtle.setNoId(); return context.endInterpretation(pyturtle); }

		inline void start() 
		{ turtle.setId(0); context.enableEarlyReturn(false); }

		inline void stop()  
		{ 
			if(!context.isEarlyReturnEnabled()){
				turtle.stop();
				if (!turtle.emptyStack()){
					printf("Turtle stack size : %lu\n",turtle.getStack().size());
					LsysError("Ill-formed string in interpretation: unmatched brackets");
				}
				else {
					timer.touch();
					LPY::plot(turtle.getScene());
					LPY::displayMessage("");
				}
			}
			else { turtle.reset(); }
		}

		inline void interpret(AxialTree::iterator m){
			 m->interpret(turtle);
			 timer.touch();
			 if (m->getClass() && m->getClass()->isPredefined()){
				LPY::plot(turtle.partialView());
			 }
			 LPY::displayMessage(m->str());
			 timer.setTimeStep(context.get_animation_timestep());
		}

		inline void incId(size_t nb = 1){
              turtle.incId(nb); 
		}

        inline void parameters(ArgList& args) {
            if (context.turtle_in_interpretation)
                args.push_front(pyturtle);
        }  
	};
void 
Lsystem::__recursiveStepInterpretation(AxialTree& workingstring,
				                const RulePtrMap& ruleset,
                                PglTurtle& t,
                                boost::python::object& pyturtle,
                                size_t maxdepth)
{



	TurtleStepInterpreter i(t,pyturtle,__context);
	__gRecursiveInterpretation<TurtleStepInterpreter>(workingstring,ruleset,i,maxdepth);
}


/*
void 
Lsystem::__recursiveInterpretation(AxialTree& workingstring,
				                const RulePtrMap& ruleset,
                                Turtle& t,
                                size_t maxdepth, bool withid)
{ 
  ContextMaintainer c(&__context);
  if( workingstring.empty()) return ;
  AxialTree::iterator _itn = workingstring.begin();
  AxialTree::const_iterator _it = workingstring.begin();
  AxialTree::const_iterator _it3 = _it;
  AxialTree::const_iterator _endit = workingstring.end();
  size_t dist = 0;
  if(withid){ 
      t.start();
      t.setId(0);
  }
  while ( _it != _endit ) {
      if ( _it->isCut() ){
	  _it3 = _it;
          _it = workingstring.endBracket(_it3);
          _itn += distance(_it3,_it);
      }
      else{
          AxialTree ltargetstring;
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end(); _it2++){
				  ArgList args;
                  if((*_it2)->match(workingstring,_it,ltargetstring,_it3,args)){
                      match = (*_it2)->applyTo(ltargetstring,args);
					  if (match) {
						dist = distance(_it,_it3);
						_it = _it3;
						_itn += dist;
						break;
					  }
                  }
          }
          if (match){
              if(maxdepth > 1) __recursiveInterpretation(ltargetstring,ruleset,t,maxdepth-1,false);
              else { 
                 for(AxialTree::iterator _itl = ltargetstring.begin();
					 _itl != ltargetstring.end(); ++_itl){
					 _itl->interpret(t);
                 } 
              }
              if(withid)t.incId(dist); 
          }
          else { 
			  ParamModule& m = *_itn; // const_cast<ParamModule&>(*_it);
			  m.interpret(t);
			  ++_it; ++_itn;
              if(withid)t.incId(); 
          }
      }
  }
  if(withid){ 
      t.stop();
      if (!t.emptyStack()){
        printf("Turtle stack size : %i\n",t.getStack().size());
	    LsysError("Ill-formed string in interpretation: unmatched brackets");
      }
  }
}

*/

AxialTree 
Lsystem::derive(  const AxialTree& wstring, 
                  size_t starting_iter , 
                  size_t nb_iter , 
                  bool previouslyinterpreted ){
  ACQUIRE_RESSOURCE
  enableEarlyReturn(false);
  if ( (__rules.empty() || wstring.empty()) && __context.return_if_no_matching )return wstring;
  ContextMaintainer c(&__context);
  AxialTree res = __derive(starting_iter,nb_iter,wstring,previouslyinterpreted);
  enableEarlyReturn(false);
  return res;
  RELEASE_RESSOURCE
}




AxialTree 
Lsystem::__derive( size_t starting_iter , 
                    size_t nb_iter , 
                    const AxialTree& wstring, 
                    bool previouslyinterpreted){
  __context.frameDisplay(true);
  AxialTree workstring = wstring;
  if(starting_iter == 0) {
	__context.setIterationNb(0);
    __apply_pre_process(workstring,false);
  }
  if ( (__rules.empty() || workstring.empty()) && __context.return_if_no_matching ){
	  if(starting_iter+nb_iter == __max_derivation) {
		__context.setIterationNb(__max_derivation);
        __apply_post_process(workstring,false);
	  }
	  return workstring;
  }
  if (!workstring.empty() && nb_iter > 0){
	bool matching = true;
	bool no_match_no_return = !__context.return_if_no_matching;
	if(!__rules.empty()||no_match_no_return){
      eDirection ndir;
      RulePtrMap production;
      bool productionHasQuery;
      RulePtrMap decomposition;
      bool decompositionHasQuery;
	  size_t i = 0;
      if(isEarlyReturnEnabled()) return workstring;
	  for(; (matching||no_match_no_return) && i < nb_iter; ++i){
		  if (__context.isSelectionAlwaysRequired() || __context.isSelectionRequested()){
			  std::vector<uint_t> sel;
			  if (__context.isSelectionRequested()){
				  sel.push_back(waitSelection(__context.getSelectionMessage()));
				  __context.selectionAquired();
			  }
			  else sel = getSelection();
			  if (!sel.empty()) {
				  uint_t added = 0;
				  size_t wstrsize = workstring.size();
				  std::sort(sel.begin(),sel.end());
				  for(std::vector<uint_t>::const_iterator it = sel.begin(); it != sel.end(); ++it)
				  {
					  if(*it < wstrsize){
						  workstring.insertAt(*it+added,ParamModule("X"));
						  added+=1;
					  }
				  }

			  }
		  }
		  __lastcomputedscene = ScenePtr();
		  __context.frameDisplay(i == (nb_iter -1));
		  __context.setIterationNb(starting_iter+i);
          __apply_pre_process(workstring,true);
		  eDirection dir = getDirection();
		  size_t group = __context.getGroup();
		  if (group > __rules.size()) LsysWarning("Group not valid.");
		  if (i == 0 || dir != ndir || group != __currentGroup || __newrules){
			  ndir = dir;
			  __currentGroup = group;
			  production = __getRules(eProduction,group,ndir,&productionHasQuery);
			  decomposition = __getRules(eDecomposition,group,ndir,&decompositionHasQuery);
			  __newrules = false;
		  }
		  if (!production.empty()){
			  if(!hasDebugger())
				  workstring = __step(workstring,production,previouslyinterpreted?false:productionHasQuery,matching,dir);
			  else workstring = __debugStep(workstring,production,previouslyinterpreted?false:productionHasQuery,matching,dir,*__debugger);
			  previouslyinterpreted = false;
		  }
		  if(!decomposition.empty()){
              bool decmatching;
              workstring = __recursiveSteps(workstring, decomposition, __decomposition_max_depth, decmatching);
              if (decmatching) matching = true;
              previouslyinterpreted = false;

			  /*bool decmatching = true;
			  for(size_t i = 0; decmatching && i < __decomposition_max_depth; i++){
				  workstring = __step(workstring,decomposition,previouslyinterpreted?false:decompositionHasQuery,decmatching,dir);
				  previouslyinterpreted = false;
				  if (decmatching) matching = true;
			  }*/
		  }
		  // Call endeach function
		  if(__context.hasEndEachFunction())
			__lastcomputedscene = __apply_post_process(workstring);
		  if(isEarlyReturnEnabled())  break;
		  if( (i+1) <  nb_iter && __context.isSelectionRequested()) {
			 __plot(workstring,true);
		  }
	  }
	  if(starting_iter+i == __max_derivation) {
		  // Call end function
		  if(__context.hasEndFunction())
			__lastcomputedscene = __apply_post_process(workstring,false);
	  }
	}
  }
  return workstring;
}

AxialTree 
Lsystem::decompose( const AxialTree& workstring  )
{
  ACQUIRE_RESSOURCE
  enableEarlyReturn(false);
  ContextMaintainer c(&__context);
  AxialTree res = __decompose(workstring);
  enableEarlyReturn(false);
  return res;
  RELEASE_RESSOURCE

}

AxialTree 
Lsystem::__decompose( const AxialTree& wstring){
  AxialTree workstring = wstring;
  if (!workstring.empty()){
    if(!__rules.empty()){

      eDirection dir = getDirection();
      size_t group = __context.getGroup();

      if (group > __rules.size()) LsysWarning("Group not valid.");

      bool decompositionHasQuery;
      RulePtrMap decomposition = __getRules(eDecomposition,group,dir,&decompositionHasQuery);

      if(!decomposition.empty()){
          bool decmatching = true;
          workstring = __recursiveSteps(workstring, decomposition, __decomposition_max_depth, decmatching);
      }
    }
  }
  return workstring;
}

void 
Lsystem::__apply_pre_process(AxialTree& workstring, bool starteach)
{
	// Call endeach function
	object result;
	switch (starteach?__context.getStartEachNbArgs():__context.getStartNbArgs()){
		default:
		case 0:
			result = starteach ? __context.startEach() : __context.start();
			break;
		case 1:
			result = starteach ? __context.startEach(workstring) : __context.start(workstring);
			break;
	}
	// Check result of starteach function
    if (result != object())
	    workstring = extract<AxialTree>(result)();
}

ScenePtr
Lsystem::__apply_post_process(AxialTree& workstring, bool endeach)
{
	// Call endeach function
	object result;
	ScenePtr rep;
	switch (endeach?__context.getEndEachNbArgs():__context.getEndNbArgs()){
		default:
		case 0:
			result = endeach ? __context.endEach() : __context.end();
				break;
		case 1:
			result = endeach ? __context.endEach(workstring) : __context.end(workstring);
				break;
		case 2:
				// if a frame should be displayed, representation is computed
				if(__context.isFrameDisplayed()) {
					__turtle_interpretation(workstring,__context.turtle,__context.pyturtle());
					rep = __context.turtle.getScene();
				}
				result = endeach ? __context.endEach(workstring,rep) : __context.end(workstring,rep);
				break;
	}
	// Check result of endeach function
	if(PyTuple_Check(result.ptr()) && len(result) >= 2){
		if (result[0] != object()) {
            workstring = extract<AxialTree>(result[0])();
        }
		if (result[1] == object()) rep = ScenePtr();
		else rep = extract<ScenePtr>(result[1])();
	}
	else if (result != object()){
		extract<ScenePtr> scextract(result);
		if(scextract.check())rep = scextract();
		else {
			workstring = extract<AxialTree>(result)();
			rep = ScenePtr();
		}
	}
	return rep;
}


void 
Lsystem::plot( AxialTree& workstring, bool checkLastComputedScene ){
    ACQUIRE_RESSOURCE
    __plot(workstring,checkLastComputedScene);
    RELEASE_RESSOURCE
}

void 
Lsystem::turtle_interpretation( AxialTree& workstring, PGL::Turtle& t )
{
  ACQUIRE_RESSOURCE
  __turtle_interpretation(workstring,t);
  RELEASE_RESSOURCE
}

ScenePtr Lsystem::sceneInterpretation( AxialTree& workstring )
{
  ACQUIRE_RESSOURCE
  __turtle_interpretation(workstring,__context.turtle,__context.pyturtle());
  return __context.turtle.getScene();
  RELEASE_RESSOURCE
}

void Lsystem::stepInterpretation(AxialTree& wstring)
{
  ACQUIRE_RESSOURCE
  if ( wstring.empty() )return;
  bool homHasQuery = false;
  RulePtrMap interpretation = __getRules(eInterpretation,__currentGroup,eForward,&homHasQuery);
  __recursiveStepInterpretation(wstring,interpretation,__context.turtle,__context.pyturtle(),__interpretation_max_depth);
  RELEASE_RESSOURCE
}


AxialTree 
Lsystem::interpret(AxialTree& wstring){
  ACQUIRE_RESSOURCE
  return __homomorphism(wstring);
  RELEASE_RESSOURCE
}

AxialTree 
Lsystem::__homomorphism(AxialTree& wstring){
  if ( wstring.empty() || __rules.empty() || 
       ( __group(0).interpretation.empty() && 
        (__group(__currentGroup).interpretation.empty()||
         __rules.size() < __currentGroup)))return wstring;
  AxialTree workstring;
  bool homHasQuery = false;  
  RulePtrMap interpretation = __getRules(eInterpretation,__currentGroup,eForward,&homHasQuery);
  if (!interpretation.empty()){
      bool decmatching;
      workstring = __recursiveSteps(wstring,interpretation,__interpretation_max_depth, decmatching);
  }
  return workstring;
}

void
Lsystem::__turtle_interpretation(AxialTree& wstring, PGL::Turtle& t, boost::python::object pyturtle){
    if (pyturtle == boost::python::object()) pyturtle = boost::python::object(boost::cref(t));
    if ( wstring.empty() )return;
    bool homHasQuery = false;
    RulePtrMap interpretation = __getRules(eInterpretation,__currentGroup,eForward,&homHasQuery);
    if (!interpretation.empty()){
      __recursiveInterpretation(wstring,interpretation,t,pyturtle,__interpretation_max_depth);
    }
    else {
        t.start();
        t.setNoId();
        AxialTree initturtle = __context.startInterpretation(pyturtle);
        for(AxialTree::iterator _itl = initturtle.begin(); _itl != initturtle.end(); ++_itl)
            _itl->interpret(t);  

        t.setId(0);
  		LPY::turtle_do_interpretation(wstring,t);

        t.setNoId();
        AxialTree finalizeturtle = __context.endInterpretation(pyturtle);
        for(AxialTree::iterator _itl = finalizeturtle.begin(); _itl != finalizeturtle.end(); ++_itl)
            _itl->interpret(t);  
    }
}

void 
Lsystem::__plot( AxialTree& workstring, bool checkLastComputedScene){
	ScenePtr result;
	if (checkLastComputedScene) {
		result = __lastcomputedscene;
	}
	if (is_null_ptr(result)) {
		__turtle_interpretation(workstring,__context.turtle,__context.pyturtle());
		result = __context.turtle.getScene();
	}
    LPY::plot(result);
	__context.postDraw();
}

// #include <plantgl/gui/viewer/pglapplication.h>

AxialTree
Lsystem::animate(const AxialTree& workstring,double dt,size_t beg,size_t nb_iter){
    ACQUIRE_RESSOURCE
    ContextMaintainer c(&__context);
    enableEarlyReturn(false);
    AxialTree tree = workstring;
    __context.set_animation_timestep(dt);
	__context.setAnimationEnabled(true);
    Sequencer timer(dt);
    timer.touch();
    __plot(tree);
    if (nb_iter > 0 && !isEarlyReturnEnabled()){
	  for (size_t i = beg; i < beg+nb_iter; i++){
	    tree = __derive(i,1,tree,true);
		if(__context.isFrameDisplayed()) {
			timer.touch();
			__plot(tree,true);
		}
        timer.setTimeStep(__context.get_animation_timestep());
        if(isEarlyReturnEnabled()) break;
	  }
	}
	__context.setAnimationEnabled(false);
    enableEarlyReturn(false);
    return tree;
    RELEASE_RESSOURCE
}

#include <plantgl/math/util_math.h>

std::string conv_number(size_t num, size_t fill){
  std::stringstream ss;
  ss.fill('0');
  ss.width(fill);
  ss << num;
  return ss.str();
}

void
Lsystem::record(const std::string& prefix,
				const AxialTree& workstring,
				size_t beg, size_t nb_iter,
                const std::string& suffix){
    ACQUIRE_RESSOURCE
    enableEarlyReturn(false);
    AxialTree tree = workstring;
    ContextMaintainer c(&__context);
	__context.setAnimationEnabled(true);
    __plot(tree);
	int fill = (int)ceil(log10((float)beg+nb_iter+1));
	LPY::saveImage(prefix+conv_number(beg,fill)+"."+suffix,suffix);
    if (nb_iter > 0){
	  for (size_t i = beg+1; i <= beg+nb_iter; i++){
		tree = __derive(i-1,1,tree,true);
		if(__context.isFrameDisplayed()) {
			__plot(tree,true);
		}
		LPY::saveImage(prefix+conv_number(i,fill)+"."+suffix,suffix);
        if(isEarlyReturnEnabled()) break;
	  }
	}
	__context.setAnimationEnabled(false);
    enableEarlyReturn(false);
    RELEASE_RESSOURCE
}


void Lsystem::enableEarlyReturn(bool val) 
{ 
	__context.enableEarlyReturn(val);
}

bool Lsystem::isEarlyReturnEnabled() 
{ 
	return __context.isEarlyReturnEnabled();
}


#ifdef MULTI_THREADED_LSYSTEM
void Lsystem::acquire() const
{
   if (!__ressource->mutex.tryLock()){
       // std::cerr << "Concurrent Access of the Lsystem" << std::endl;
	   LsysError("Concurrent Access of the Lsystem");
   }
}

void Lsystem::release() const
{    
   __ressource->mutex.unlock();
}
#endif

bool Lsystem::isRunning() const
{
#ifdef MULTI_THREADED_LSYSTEM
   if (!__ressource->mutex.tryLock()) return true;
   __ressource->mutex.unlock();
   return false;
#else
   return false;
#endif
}

void 
Lsystem::forceRelease(){
#ifdef MULTI_THREADED_LSYSTEM
	if(isRunning()) release();
#endif
}
/*---------------------------------------------------------------------------*/

Lsystem::Debugger::~Debugger()  { }

/*---------------------------------------------------------------------------*/
