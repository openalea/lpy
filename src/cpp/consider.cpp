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

#include "consider.h"
#include "axialtree.h"
#include "tracker.h"

// using namespace boost::python;
LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

bool Module::isConsidered(const ConsiderFilterPtr filter) const
{ 
	if(is_null_ptr(filter)) return true;
	return filter->isConsidered(*this); 
}

bool Module::isIgnored(const ConsiderFilterPtr filter) const
{ 
	if(is_null_ptr(filter)) return false;
	return filter->isIgnored(*this); 
}


/*---------------------------------------------------------------------------*/

/*
static std::stack<ConsiderFilterPtr> CONSIDERFILTER_STACK;
static ConsiderFilterPtr CURRENT_CONSIDERFILTER = NULL;

ConsiderFilterPtr 
ConsiderFilter::current() { return CURRENT_CONSIDERFILTER; }

void makeFilterCurrent(ConsiderFilter * filter = NULL)
{
	// printf("make filter current : %i\n",filter);
	CONSIDERFILTER_STACK.push(CURRENT_CONSIDERFILTER);
	CURRENT_CONSIDERFILTER = ConsiderFilterPtr(filter);
}

void makeFilterDone(ConsiderFilter * filter = NULL)
{
	// printf("done filter current : %i\n",filter);
	if (filter != CURRENT_CONSIDERFILTER.get()) 
		LsysError("Consider Filter to pop is different from current.");
	else {
		if (CONSIDERFILTER_STACK.empty()) 
			LsysError("Consider Filter stack empty. Cannot done");
		else {
			CURRENT_CONSIDERFILTER = CONSIDERFILTER_STACK.top();
			CONSIDERFILTER_STACK.pop();
		}
	}
}

void 
ConsiderFilter::makeCurrent()
{
	makeFilterCurrent(this);
}

void ConsiderFilter::done()
{
	makeFilterDone(this);
}

bool ConsiderFilter::isCurrent()
{
	return CURRENT_CONSIDERFILTER.get() == this;
}

void 
ConsiderFilter::makeNoneCurrent()
{
	makeFilterCurrent();
}

void ConsiderFilter::doneNone()
{
	makeFilterDone();
}

bool ConsiderFilter::isNoneCurrent()
{
	return !is_null_ptr(CURRENT_CONSIDERFILTER);
}
*/
/*---------------------------------------------------------------------------*/

ConsiderFilter::ConsiderFilter(const std::string& modules, eConsiderMethod method):
RefCountObject(), __method(method) {
  IncTracker(ConsiderFilter)
  if(!modules.empty()){
    AxialTree t(modules);
    for(AxialTree::const_iterator _it = t.begin(); _it != t.end(); ++_it)
      __keyword[_it->getClass()->getId()] = _it->getClass();
  }
}


ConsiderFilter::ConsiderFilter(const ModuleClassList& modules, eConsiderMethod method):
RefCountObject(), __method(method) {
  IncTracker(ConsiderFilter)
  if(!modules.empty()){
    for(ModuleClassList::const_iterator _it = modules.begin(); _it != modules.end(); ++_it)
      __keyword[(*_it)->getId()] = (*_it);
  }
}

ConsiderFilter::~ConsiderFilter(){
  DecTracker(ConsiderFilter)    
}

bool
ConsiderFilter::isIgnored(const ModuleClassPtr moduleclass) const{
  if(__keyword.empty())return false;
  ModuleClassSet::const_iterator _it = __keyword.find(moduleclass->getId());
  if(__method == eIgnore) return _it != __keyword.end();
  else { 
	  if (_it != __keyword.end()) return false;
	  else return !moduleclass->isBracket(); // by default we consider always bracket
	  // return _it == __keyword.end();
  }
}

std::string
ConsiderFilter::keyword() const{
  if(__keyword.empty())return std::string("");
  std::string res;
  for(ModuleClassSet::const_iterator _it = __keyword.begin();
	  _it != __keyword.end(); ++_it){
		  if (_it != __keyword.begin()) res += " ";
		  res += _it->second->name;
  }
  return res;
}	

std::string
ConsiderFilter::str() const{
  if(__keyword.empty())return std::string("");
  std::string res = (__method == eConsider?"consider: ":"ignore: ");
  res += keyword();
  return res;
}	

ConsiderFilterPtr ConsiderFilter::ignorePredefined() 
{   
    ModuleClassList mcl(ModuleClass::getPredefinedClasses());
    std::vector<std::string> toremove;
    toremove.push_back("");
    toremove.push_back("[");
    toremove.push_back("]");
    for(std::vector<std::string>::const_iterator itn = toremove.begin(); itn != toremove.end(); ++itn){
        ModuleClassList::iterator itm = mcl.begin();
        for(; itm != mcl.end() && (*itm)->name != *itn; ++itm);
        if(itm != mcl.end()) mcl.erase(itm);
    }
    return ConsiderFilterPtr(new ConsiderFilter(mcl,eIgnore)); 
}
/*---------------------------------------------------------------------------*/
/*
ConsiderFilterMaintainer::ConsiderFilterMaintainer(ConsiderFilterPtr _filter) : 
    to_set(!(is_valid_ptr(_filter)?_filter->isCurrent():ConsiderFilter::isNoneCurrent())), filter(_filter)
{ 
	if (to_set) {
		if(is_valid_ptr(filter))filter->makeCurrent();
		else ConsiderFilter::makeNoneCurrent();
	}
}

ConsiderFilterMaintainer::~ConsiderFilterMaintainer() { 
	if (to_set) {
		if(is_valid_ptr(filter))filter->done(); 
		else ConsiderFilter::doneNone();
	}
}
*/