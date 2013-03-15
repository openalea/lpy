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

#ifndef __lpy_axialtree_h__
#define __lpy_axialtree_h__

#include <vector>
#include "module.h"
#include "patternmodule.h"
#include "abstractlstring.h"
#include "global.h"


LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class PatternString;

/*---------------------------------------------------------------------------*/

class LPY_API AxialTree  : public AbstractLString<ParamModule>  {
public:
  typedef AbstractLString<ParamModule> BaseType;

  AxialTree();
  AxialTree(const AxialTree&);
  AxialTree(const std::string&);
  AxialTree(const ParamModule&);
  AxialTree(const boost::python::list&);
  AxialTree(const boost::python::tuple&);
  AxialTree(const_iterator beg, const_iterator end);

  ~AxialTree();
  

	std::string repr() const;

    inline std::string str() const { return str_slice(const_begin(),const_end()); }
	inline std::string str_slice(int beg, int end) const
	{ const_iterator begit, endit; getValidIterators(beg,end,begit,endit); return str_slice(begit,endit); }

	std::string str_slice(const_iterator beg, const_iterator end) const;

	size_t count(const std::string& name) const;
	size_t count(const std::string& name, size_t nbparam) const;
	size_t count(const ParamModule& module) const;

	inline const_iterator find(const std::string& name) const
	{ return find(name,const_begin(),const_end()); }

	inline const_iterator find(const std::string& name, 
					    const_iterator start) const
	{ return find(name,start,end()); }

	const_iterator find(const std::string& name, 
					    const_iterator start,
						const_iterator stop) const;

	inline const_iterator find(const std::string& name, size_t nbparam) const
	{ return find(name,nbparam,const_begin(),const_end()); }

	inline const_iterator find(const std::string& name, size_t nbparam, 
					    const_iterator start) const
	{ return find(name,nbparam,start,const_end()); }

	const_iterator find(const std::string& name, size_t nbparam, 
					    const_iterator start,
						const_iterator stop) const;

	inline const_iterator find(const PatternModule& pattern) const
	{ return find(pattern.name(),pattern.size(),const_begin(),const_end()); }

	inline const_iterator find(const PatternModule& pattern, 
					    const_iterator start) const
	{ return find(pattern.name(),pattern.size(),start,const_end()); }

	inline const_iterator find(const PatternModule& pattern, 
					    const_iterator start,
						const_iterator stop) const
	{ return find(pattern.name(),pattern.size(),start,stop); }

	inline const_iterator find(const PatternString& pattern) const
	{ return find(pattern,begin(),const_end()); }

	inline const_iterator find(const PatternString& pattern, 
					    const_iterator start) const
	{ return find(pattern,start,const_end()); }

	const_iterator find(const PatternString& pattern, 
					    const_iterator start,
						const_iterator stop) const;

	inline bool match(const PatternModule& pattern, 
			   const_iterator pos) const
	{ return pos->match(pattern); }

	inline bool match(const PatternString& pattern, 
			   const_iterator  pos) const
   { AxialTree::const_iterator res; return match(pattern,pos,res); }

	inline bool match(const PatternString& pattern, 
			   const_iterator  pos,
			   const_iterator& resultingpos) const
	{ ArgList params; return match(pattern,pos,resultingpos,params); }

	inline bool match(const PatternString& pattern, 
			   const_iterator  pos,
			   const_iterator& resultingpos,
			   ArgList& params) const
    { AxialTree::const_iterator last_matched;  return match(pattern,pos,resultingpos,last_matched,params);  }

	bool match(const PatternString& pattern, 
			   const_iterator  pos,
			   const_iterator& resultingpos,
			   const_iterator& last_matched,
			   ArgList& params) const;

	bool reverse_match(const PatternString& pattern, 
			   const_iterator  pos) const;

	bool reverse_match(const PatternString& pattern, 
			   const_iterator  pos,
			   const_iterator& resultingpos,
			   ArgList& params) const;

	bool reverse_match(const PatternString& pattern, 
			   const_iterator  pos,
			   const_iterator& resultingpos) const;

	bool rightmatch(const PatternString& pattern, 
					const_iterator pos) const;

	bool rightmatch(const PatternString& pattern, 
					const_iterator pos,
					const_iterator& resultingpos) const;

	bool rightmatch(const PatternString& pattern, 
					const_iterator pos,
					const_iterator& resultingpos,
					ArgList& params) const;

	bool rightmatch(const PatternString& pattern, 
					const_iterator pos,
					const_iterator last_matched,
					const_iterator& resultingpos,
					ArgList& params) const;

	bool leftmatch(const PatternString& pattern,  
					const_iterator pos,
					const_iterator& resultingpos,
					ArgList& params) const;

	bool leftmatch(const PatternString& pattern,  
					const_iterator pos,
					const_iterator& resultingpos) const;

	bool leftmatch(const PatternString& pattern,  
					const_iterator pos) const;

	const_iterator rightfind(const PatternString& pattern,
							 const_iterator start,
						     const_iterator stop) const;

	inline const_iterator rightfind(const PatternString& pattern,
							 const_iterator start) const
	{ return rightfind(pattern,start,const_end()); }

	inline const_iterator rightfind(const PatternString& pattern) const
	{ return rightfind(pattern,const_begin(),const_end()); }

	const_iterator leftfind(const PatternString& pattern,
							 const_iterator start,
						     const_iterator stop) const;

	inline const_iterator leftfind(const PatternString& pattern,
							 const_iterator start) const
	{ return leftfind(pattern,start,const_end()); }

	inline const_iterator leftfind(const PatternString& pattern) const
	{ return leftfind(pattern,const_begin(),const_end()); }

	AxialTree replace(const PatternModule&, const ParamModule&) const;
	AxialTree replace(const PatternModule&, const AxialTree&) const;
	AxialTree replace(const PatternString&, const AxialTree&) const;

};


/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
