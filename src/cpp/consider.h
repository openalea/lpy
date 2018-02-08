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

#ifndef __CONSIDER_H__
#define __CONSIDER_H__

#include "global.h"
#include "module.h"

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class ConsiderFilter;
typedef RCPtr<ConsiderFilter> ConsiderFilterPtr;

class LPY_API ConsiderFilter  : public TOOLS(RefCountObject) {
public:

  ConsiderFilter(const std::string& modules, eConsiderMethod method = eConsider);

  ConsiderFilter(const ModuleClassList& modules, eConsiderMethod method = eConsider);

  ~ConsiderFilter();

  // isConsidered
  inline bool isConsidered(const std::string& module) const
  { return !isIgnored(module); }

  inline bool isConsidered(const Module& module) const
  { return !isIgnored(module.getClass()); }

  inline bool isConsidered(const ModuleClassPtr moduleclass) const
  { return !isIgnored(moduleclass); }

  // isIgnored
  inline bool isIgnored(const std::string& module) const
  { return isIgnored(ModuleClassTable::get().getClass(module)); }

  inline bool isIgnored(const Module& module) const
  { return isIgnored(module.getClass()); }

  bool isIgnored(const ModuleClassPtr module) const;

  inline bool ignoring() const { return __method == eIgnore; }
  inline eConsiderMethod method() const { return __method; }

  std::string keyword() const;

  std::string str() const;
  inline const char * c_str() const { return str().c_str(); }

/*  
  void makeCurrent();
  void done();
  bool isCurrent();

  static void makeNoneCurrent();
  static void doneNone();
  static bool isNoneCurrent();

  static ConsiderFilterPtr current();
*/

  static ConsiderFilterPtr consider(const std::string& modules)
  {   
	  if (modules.empty()) return ConsiderFilterPtr();
	  return ConsiderFilterPtr(new ConsiderFilter(modules)); 
  }

  static ConsiderFilterPtr ignore(const std::string& modules) 
  {   
      if (modules.empty()) return ConsiderFilterPtr();
      return ConsiderFilterPtr(new ConsiderFilter(modules,eIgnore)); 
  }

  static ConsiderFilterPtr ignorePredefined() ;

protected:
  /// attributes for ignore and consider
  typedef pgl_hash_map<size_t,ModuleClassPtr> ModuleClassSet;
  ModuleClassSet __keyword;
  eConsiderMethod __method;
};


/*
struct ConsiderFilterMaintainer {
    bool to_set;
    ConsiderFilterPtr filter;

    ConsiderFilterMaintainer(ConsiderFilterPtr _filter);
    ~ConsiderFilterMaintainer() ;

};
*/

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
