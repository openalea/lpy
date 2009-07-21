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
  __homhasquery(false)
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
        case eHomomorphism:
            return homomorphism;
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
        case eHomomorphism:
            return homomorphism;
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
        case eHomomorphism:
            return __homhasquery;
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
__homomorphism_max_depth(1),
__currentGroup(0),
__context(),
__early_return(false),
__early_return_mutex()
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
__homomorphism_max_depth(1),
__context(),
__early_return(false),
__early_return_mutex()
#ifdef MULTI_THREADED_LSYSTEM
,__ressource(new LsysRessource())
#endif
{
  IncTracker(Lsystem)
  PRINT_RESSOURCE("create")
  read(filename);
}

Lsystem::Lsystem(const Lsystem& lsys):
__rules(lsys.__rules),
__max_derivation(lsys.__max_derivation),
__decomposition_max_depth(lsys.__decomposition_max_depth),
__homomorphism_max_depth(lsys.__homomorphism_max_depth),
__context(lsys.__context),
__early_return(false),
__early_return_mutex()
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
    __homomorphism_max_depth = lsys.__homomorphism_max_depth;
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
  __homomorphism_max_depth = 1;
  __context.clear();
  __early_return = false;
}

std::string 
Lsystem::str() const {
  ACQUIRE_RESSOURCE
  std::stringstream s;
  s << "Lsystem:\n";
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

  if(!__context.empty())
	s << (__context.ignoring()?"ignore":"consider") << ": " << __context.keyword() << '\n';
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
          if(!g->homomorphism.empty()){
              s << "homomorphism:\n";
              if (gid == 0)
                s << "maximum depth:"  << __homomorphism_max_depth << '\n';
              for (RuleSet::const_iterator i = g->homomorphism.begin();
                  i != g->homomorphism.end(); i++){
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
  if(!__context.empty())
	s << (__context.ignoring()?"ignore":"consider") << ": " << __context.keyword() << '\n';
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
          if(!g->homomorphism.empty()){
              s << "homomorphism:\n";
              if (gid == 0)
                s << "maximum depth:"  << __homomorphism_max_depth << '\n';
              for (RuleSet::iterator i = g->homomorphism.begin(); i != g->homomorphism.end(); ++i){
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
Lsystem::compiled(){
  ACQUIRE_RESSOURCE
  RuleSet::const_iterator i;
  for (RuleGroupList::const_iterator g = __rules.begin(); g != __rules.end(); ++g)
  {
      for ( i = g->production.begin();    i != g->production.end(); ++i)
          if(!i->compiled())return false;
      for ( i = g->decomposition.begin(); i != g->decomposition.end(); ++i)
          if(!i->compiled())return false;
      for ( i = g->homomorphism.begin();  i != g->homomorphism.end(); ++i)
          if(!i->compiled())return false;
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
      for (i = g->homomorphism.begin();  i != g->homomorphism.end(); ++i)
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
      for ( i = g->homomorphism.begin();  i != g->homomorphism.end(); ++i)
          i->importPyFunction();
  }
}

#include <fstream>

void 
Lsystem::read(const std::string& filename){
  clear();
  std::ifstream file(filename.c_str());
  if(file){
    setFilename(filename);
	std::string content;
	char text[1001];
	text[1000] = '\0';
	while(!file.eof()){
	  file.getline(text,1000);
	  content += std::string(text)+"\n";
	}
	set(content);
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
Lsystem::__addProdRule( const std::string& code, size_t groupid, int lineno ){
  RuleGroup& group = __group(groupid);
  LsysRule r(group.production.size(),groupid,'p',lineno);
  r.set(code);
  group.production.push_back(r);
  if (r.hasQuery())group.__prodhasquery = true;
  return *(group.production.end()-1);
}

LsysRule& 
Lsystem::__addDecRule( const std::string& code, size_t groupid , int lineno ){
  RuleGroup& group = __group(groupid);
  LsysRule r(group.decomposition.size(),groupid,'d',lineno);
  r.set(code);
  group.decomposition.push_back(r);
  if (r.hasQuery())group.__dechasquery = true;
  return *(group.decomposition.end()-1);
}

LsysRule&
Lsystem::__addHomRule( const std::string& code, size_t groupid, int lineno ){
  RuleGroup& group = __group(groupid);
  LsysRule r(group.homomorphism.size(),groupid,'h',lineno);
  r.set(code);
  if (!r.isContextFree())LsysWarning("Homomorphism rules should be context free. Contexts not supported for multiple iterations.");
  group.homomorphism.push_back(r);
  if (r.hasQuery())group.__homhasquery = true;
  return *(group.homomorphism.end()-1);
}

LsysRule&
Lsystem::__addRule( const std::string& rule, int type, size_t group, int lineno ){
  switch(type){
  case 1:
	return __addDecRule(rule,group,lineno);
	break;
  case 2:
	return __addHomRule(rule,group,lineno);
	break;
  default:
	return __addProdRule(rule,group,lineno);
	break;
  }
}

void 
Lsystem::addProdRule( const std::string& code, size_t group ){
  ACQUIRE_RESSOURCE
  ContextMaintainer m(&__context);
  LsysRule& r = __addProdRule(code,group);
  r.compile();
  RELEASE_RESSOURCE
}

void 
Lsystem::addDecRule( const std::string& code, size_t group ){
  ACQUIRE_RESSOURCE
  ContextMaintainer m(&__context);
  LsysRule& r = __addDecRule(code,group);
  r.compile();
  RELEASE_RESSOURCE
}

void 
Lsystem::addHomRule( const std::string& code, size_t group ){
  ACQUIRE_RESSOURCE
  ContextMaintainer m(&__context);
  LsysRule& r = __addHomRule(code,group);
  r.compile();
  RELEASE_RESSOURCE
}

void 
Lsystem::addRule(  const LsysRule& rule, int type, size_t groupid){
  switch(type){
  case 1:
    __group(groupid).decomposition.push_back(rule);
    if (rule.hasQuery())__group(groupid).__dechasquery = true;
    break;
  case 2:
    if (!rule.isContextFree())LsysWarning("Homomorphism rules should be context free. Contexts not supported for multiple iterations.");
    __group(groupid).homomorphism.push_back(rule);
    if (rule.hasQuery())__group(groupid).__homhasquery = true;
	break;
  default:
    __group(groupid).production.push_back(rule);
    if (rule.hasQuery())__group(groupid).__prodhasquery = true;
	break;
  }
}

void Lsystem::addRule( const std::string& rule, int type, size_t group ){
    __addRule(rule,type,group);
}

bool 
Lsystem::empty( ) const {
  ACQUIRE_RESSOURCE
  return __rules.empty();
  RELEASE_RESSOURCE
}

size_t 
Lsystem::nbProductionRules( size_t group ) const {
  ACQUIRE_RESSOURCE
  if (__rules.size() < group) return 0;
  return __group(group).production.size();
  RELEASE_RESSOURCE
}

size_t 
Lsystem::nbDecompositionRules( size_t group ) const {
  ACQUIRE_RESSOURCE
  if (__rules.size() < group) return 0;
  return __group(group).decomposition.size();
  RELEASE_RESSOURCE
}

size_t Lsystem::nbHomomorphismRules( size_t group ) const {
  ACQUIRE_RESSOURCE
  if (__rules.size() < group) return 0;
  return __group(group).homomorphism.size();
  RELEASE_RESSOURCE
}

size_t Lsystem::nbTotalRules(  ) const {
  ACQUIRE_RESSOURCE
  size_t nbrules = 0;
  for(RuleGroupList::const_iterator it = __rules.begin(); it != __rules.end(); ++it)
        nbrules += it->production.size()+it->decomposition.size()+it->homomorphism.size();
  return nbrules;
  RELEASE_RESSOURCE
}

size_t Lsystem::nbGroups( ) const {
  ACQUIRE_RESSOURCE
  return __rules.size();
  RELEASE_RESSOURCE
}


void 
Lsystem::setAxiom( const AxialTree& axiom ){
  ACQUIRE_RESSOURCE
  __axiom = axiom;
  RELEASE_RESSOURCE
}


const AxialTree& 
Lsystem::getAxiom( ) const {
  ACQUIRE_RESSOURCE
  return __axiom;
  RELEASE_RESSOURCE
}

RulePtrMap Lsystem::__getRules(eRuleType type, size_t groupid, eDirection direction, bool * hasQuery)
{
    if(hasQuery)*hasQuery = false;
    if (groupid >= __rules.size()) return RulePtrMap();
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
    return RulePtrMap(result);
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
  if ( query )__interpret(workingstring,__context.envturtle);
  debugger.begin(workingstring,direction);
  if ( direction == eForward){
      AxialTree::const_iterator _it = workingstring.begin();
      AxialTree::const_iterator _it3 = _it;
      AxialTree::const_iterator _endit = workingstring.end();

      while ( _it != _endit ) {
          if ( _it->isCut() )
              _it = workingstring.endBracket(_it);
          else{
              bool match = false;
			  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
              for(RulePtrSet::const_iterator _it2 = mruleset.begin();
                  _it2 != mruleset.end(); _it2++){
					  ArgList args;
					  size_t prodlength;
                      if((*_it2)->match(workingstring,_it,targetstring,_it3,args)){
                          match = (*_it2)->applyTo(targetstring,args,&prodlength);
						  if(match) { 
							  debugger.total_match(workingstring,_it,_it3,targetstring,prodlength,*_it2,args);
							  _it = _it3; break; 
						  }
						  else debugger.partial_match(workingstring,_it,_it3,targetstring,*_it2,args);
                      }
              }
              if (!match){
                 targetstring.push_back(_it);
				 debugger.identity(workingstring,_it,targetstring);
				 ++_it;
              }
              else matching = true;
          }
      }
  }
  else {
      AxialTree::const_iterator _it = workingstring.end()-1;
      AxialTree::const_iterator _it3 = _it;
      AxialTree::const_iterator _endit = workingstring.begin();
      bool ending = false;
      do {
          if (_it == _endit) ending = true;
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end();  _it2++){
				  ArgList args;
				  size_t prodlength;
                  if((*_it2)->reverse_match(workingstring,_it,targetstring,_it3,args)){
                      match = (*_it2)->reverseApplyTo(targetstring,args,&prodlength);
                      if(match) { 							  
						  debugger.total_match(workingstring,_it==_endit?_it3:_it3+1,_it+1,targetstring,prodlength,*_it2,args);
						  _it = _it3; break; 
					  }
 					  else debugger.partial_match(workingstring,_it==_endit?_it3:_it3+1,_it+1,targetstring,*_it2,args);
                 }
          }
          if (!match){
              targetstring.push_front(_it);
			  debugger.identity(workingstring,_it,targetstring);
              if(_it != _endit) --_it;
          }
          else matching = true;
      } while ( !ending );
  }
  debugger.end(targetstring);
  return targetstring;
}



AxialTree 
Lsystem::__step(AxialTree& workingstring,
				const RulePtrMap& ruleset,
				bool query,
				bool& matching,
                eDirection direction){
  ContextMaintainer c(&__context);
  matching = false;
  if( workingstring.empty()) return workingstring;
  AxialTree targetstring;
  targetstring.reserve(workingstring.size());
  if ( query )__interpret(workingstring,__context.envturtle);
  if ( direction == eForward){
      AxialTree::const_iterator _it = workingstring.begin();
      AxialTree::const_iterator _it3 = _it;
      AxialTree::const_iterator _endit = workingstring.end();

      while ( _it != _endit ) {
          if ( _it->isCut() )
              _it = workingstring.endBracket(_it);
          else{
              bool match = false;
			  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
              for(RulePtrSet::const_iterator _it2 = mruleset.begin();
                  _it2 != mruleset.end(); _it2++){
					  ArgList args;
                      if((*_it2)->match(workingstring,_it,targetstring,_it3,args)){
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
      AxialTree::const_iterator _endit = workingstring.begin();
      bool ending = false;
      do {
          if (_it == _endit) ending = true;
          bool match = false;
		  const RulePtrSet& mruleset = ruleset[_it->getClassId()];
          for(RulePtrSet::const_iterator _it2 = mruleset.begin();
              _it2 != mruleset.end();  _it2++){
				  ArgList args;
                  if((*_it2)->reverse_match(workingstring,_it,targetstring,_it3,args)){
                      match = (*_it2)->reverseApplyTo(targetstring,args);
                      if(match) { _it = _it3; break; }
                  }
          }
          if (!match){
              targetstring.push_front(_it);
              if(_it != _endit) --_it;
          }
          else matching = true;
      } while ( !ending );
  }
  return targetstring;
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
  if ( query )LPY::interpret(workingstring,__context.turtle);
  AxialTree::const_iterator _it = workingstring.begin();
  AxialTree::const_iterator _it3 = _it;
  AxialTree::const_iterator _endit = workingstring.end();
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
                  if((*_it2)->match(workingstring,_it,targetstring,_it3,args)){
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
                          size_t maxdepth)
{
  ContextMaintainer c(&__context);
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
					  if(match) { _it = _it3; break; }
                  }
          }
          if (match){
              if(maxdepth >1) {
                  targetstring += __recursiveSteps(ltargetstring,ruleset,maxdepth-1);
              }
              else targetstring += ltargetstring;
          }
          else { targetstring.push_back(_it);++_it; }
      }
  }
  return targetstring;
}

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

AxialTree 
Lsystem::iterate( size_t starting_iter , 
                  size_t nb_iter , 
                  const AxialTree& wstring, 
                  bool previouslyinterpreted ){
  ACQUIRE_RESSOURCE
  enableEarlyReturn(false);
  if ( __rules.empty() || wstring.empty() )return wstring;
  ContextMaintainer c(&__context);
  AxialTree res = __iterate(starting_iter,nb_iter,wstring,previouslyinterpreted);
  enableEarlyReturn(false);
  return res;
  RELEASE_RESSOURCE
}

AxialTree 
Lsystem::__iterate( size_t starting_iter , 
                    size_t nb_iter , 
                    const AxialTree& wstring, 
                    bool previouslyinterpreted){
  if(starting_iter == 0) __context.start();
  if ( __rules.empty() || wstring.empty() ){
	  if(starting_iter+nb_iter == __max_derivation) __context.end();
	  return wstring;
  }
  AxialTree workstring = wstring;
  if (!workstring.empty() && nb_iter > 0){
	bool matching = true;
	bool no_match_no_return = !__context.return_if_no_matching;
	if(!__rules.empty()){
      eDirection ndir;
      RulePtrMap production;
      bool productionHasQuery;
      RulePtrMap decomposition;
      bool decompositionHasQuery;
	  size_t i = 0;
	  for(; (matching||no_match_no_return) && i < nb_iter; ++i){
        if(isEarlyReturnEnabled()) break;
        if (__context.isSelectionRequired()){
            std::vector<uint_t> sel = getSelection();
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
		__context.setIterationNb(starting_iter+i);
		__context.startEach();
        eDirection dir = getDirection();
        size_t group = __context.getGroup();
        if (group > __rules.size()) LsysError("Group not valid.");
        if (i == 0 || dir != ndir || group != __currentGroup){
            ndir = dir;
            __currentGroup = group;
            production = __getRules(eProduction,group,ndir,&productionHasQuery);
            decomposition = __getRules(eDecomposition,group,ndir,&decompositionHasQuery);
        }
        if (!production.empty()){
            if(!hasDebugger())
				workstring = __step(workstring,production,previouslyinterpreted?false:productionHasQuery,matching,dir);
			else workstring = __debugStep(workstring,production,previouslyinterpreted?false:productionHasQuery,matching,dir,*__debugger);
            previouslyinterpreted = false;
        }
		if(!decomposition.empty()){
		  bool decmatching = true;
		  for(size_t i = 0; decmatching && i < __decomposition_max_depth; i++){
			workstring = __step(workstring,decomposition,previouslyinterpreted?false:decompositionHasQuery,decmatching,dir);
            previouslyinterpreted = false;
            if (decmatching) matching = true;
		  }
		}
		switch (__context.getEndEachNbArgs()){
			default:
			case 0:
				__context.endEach();
				break;
			case 1:
				__context.endEach(workstring);
				break;
			case 2:
				__interpret(workstring,__context.turtle);
				__context.endEach(workstring,__context.turtle.getScene());
				break;
	        }

	  }
	  if(starting_iter+i == __max_derivation) __context.end();
	}
  }
  return workstring;
}


void 
Lsystem::plot( AxialTree& workstring ){
    ACQUIRE_RESSOURCE
    __plot(workstring);
    RELEASE_RESSOURCE
}

void 
Lsystem::interpret( AxialTree& workstring, PGL::Turtle& t )
{
  ACQUIRE_RESSOURCE
  __interpret(workstring,t);
  RELEASE_RESSOURCE
}

ScenePtr Lsystem::sceneInterpretation( AxialTree& workstring )
{
  ACQUIRE_RESSOURCE
  __interpret(workstring,__context.turtle);
  return __context.turtle.getScene();
  RELEASE_RESSOURCE
}



AxialTree 
Lsystem::homomorphism(AxialTree& wstring){
  ACQUIRE_RESSOURCE
  return __homomorphism(wstring);
  RELEASE_RESSOURCE
}

AxialTree 
Lsystem::__homomorphism(AxialTree& wstring){
  if ( wstring.empty() || __rules.empty() || 
       ( __group(0).homomorphism.empty() && 
        (__group(__currentGroup).homomorphism.empty()||
         __rules.size() < __currentGroup)))return wstring;
  AxialTree workstring;
  bool homHasQuery = false;  
  RulePtrMap homomorphism = __getRules(eHomomorphism,__currentGroup,eForward,&homHasQuery);
  if (!homomorphism.empty()){
      workstring = __recursiveSteps(wstring,homomorphism,__homomorphism_max_depth);
  }
  return workstring;
}

void
Lsystem::__interpret(AxialTree& wstring, PGL::Turtle& t){
    if ( wstring.empty() )return;
    bool homHasQuery = false;
    RulePtrMap homomorphism = __getRules(eHomomorphism,__currentGroup,eForward,&homHasQuery);
    if (!homomorphism.empty()){
      __recursiveInterpretation(wstring,homomorphism,t,__homomorphism_max_depth);
    }
    else {
		LPY::interpret(wstring,t);
    }
}

void 
Lsystem::__plot( AxialTree& workstring ){
    __interpret(workstring,__context.turtle);
    LPY::plot(__context.turtle.getScene());
}

#include <plantgl/tool/sequencer.h>
// #include <plantgl/gui/viewer/pglapplication.h>

AxialTree
Lsystem::animate(const AxialTree& workstring,double dt,size_t beg,size_t nb_iter){
    ACQUIRE_RESSOURCE
    enableEarlyReturn(false);
    AxialTree tree = workstring;
    __context.set_animation_timestep(dt);
    Sequencer timer(dt);
    timer.touch();
    ContextMaintainer c(&__context);
    __plot(tree);
    if (nb_iter > 0 && !isEarlyReturnEnabled()){
	  for (size_t i = beg; i < beg+nb_iter; i++){
	    tree = __iterate(i,1,tree,true);
		timer.touch();
        timer.setTimeStep(__context.get_animation_timestep());
		__plot(tree);
        if(isEarlyReturnEnabled()) break;
	  }
	}
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
				size_t beg, size_t nb_iter){
    ACQUIRE_RESSOURCE
    enableEarlyReturn(false);
    AxialTree tree = __axiom;
    ContextMaintainer c(&__context);
    if (beg > 0) tree = __iterate(0,beg,tree);
    __plot(tree);
	int fill = (int)ceil(log10((float)beg+nb_iter+1));
	LPY::saveImage(prefix+conv_number(beg,fill)+".png");
    if (nb_iter > 0){
	  for (size_t i = beg+1; i <= beg+nb_iter; i++){
		tree = __iterate(i-1,1,tree,true);
        __plot(tree);
		LPY::saveImage(prefix+conv_number(i,fill)+".png");
        if(isEarlyReturnEnabled()) break;
	  }
	}
    enableEarlyReturn(false);
    RELEASE_RESSOURCE
}


void Lsystem::enableEarlyReturn(bool val) 
{ 
    QWriteLocker ml(&__early_return_mutex);
    __early_return = val; 
}

bool Lsystem::isEarlyReturnEnabled() 
{ 
    QReadLocker ml(&__early_return_mutex);
    return __early_return; 
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

/*---------------------------------------------------------------------------*/

Lsystem::Debugger::~Debugger()  { }

/*---------------------------------------------------------------------------*/
