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

#ifndef __PGL_AXIALTREE_H__
#define __PGL_AXIALTREE_H__

#include <vector>
#include "mod.h"

#define USE_SHARED_DATA

#ifdef USE_SHARED_DATA
#include <QtCore/QSharedData>
#endif

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

LPY_API enum eDirection {
      eForward,
      eBackward
} ;

/*---------------------------------------------------------------------------*/

class LPY_API AxialTree {
protected:
  
  typedef std::vector<ParamModule> ModuleList;

public:
  /// An iterator used to iterate through an AxialTree.
  typedef ModuleList::iterator iterator;

  /// A const iterator used to iterate through an AxialTree.
  typedef ModuleList::const_iterator const_iterator;

  AxialTree();
  AxialTree(const AxialTree&);
  AxialTree(const std::string&);
  AxialTree(const ParamModule&);
  AxialTree(const boost::python::list&);
  AxialTree(const boost::python::tuple&);
  AxialTree(const_iterator beg, const_iterator end);

  ~AxialTree();
  AxialTree& operator=(const AxialTree&);
  
  static AxialTree QueryTree(const std::string&);


  /// Returns an iterator at the beginning of \e self.
  inline iterator begin()
   { return __string().begin(); }

  /// Returns an iterator at the beginning of \e self.
  inline const_iterator begin() const
   { return __conststring().begin(); }

  /// Returns an iterator at the beginning of \e self.
  inline const_iterator const_begin() const
   { return __conststring().begin(); }

  /// Returns an iterator at the end of \e self.
  inline iterator end()
  { return __string().end(); }

  /// Returns a const iterator at the end of \e self.
  inline const_iterator end() const
  { return __conststring().end(); }

  /// Returns a const iterator at the end of \e self.
  inline const_iterator const_end() const
  { return __conststring().end(); }

  inline bool isEnd(ModuleList::const_iterator it) const
  { return (const_end() == it); }

  inline bool isBegin(ModuleList::const_iterator it) const
  { return (const_begin() == it); }


	AxialTree& operator+=(const ParamModule&);
	AxialTree& operator+=(const AxialTree&);
	AxialTree& operator+=(const std::string&);
	AxialTree& operator+=(const boost::python::list&);
	AxialTree& operator+=(const boost::python::tuple&);

	AxialTree operator+(const ParamModule&);
	AxialTree operator+(const AxialTree&);
	AxialTree operator+(const std::string&);
	AxialTree operator+(const boost::python::list&);
	AxialTree operator+(const boost::python::tuple&);

	AxialTree& prepend(const ParamModule&);
	AxialTree& prepend(const AxialTree&);
	AxialTree& prepend(const std::string&);
	AxialTree& prepend(const boost::python::list&);
	AxialTree& prepend(const boost::python::tuple&);

	AxialTree operator*(int) const;
	AxialTree& operator*=(int);

	const ParamModule& operator[](size_t i) const;
	const ParamModule& getAt(int i) const;
	AxialTree getRange(int i, int j) const;

	void setAt(int i, const ParamModule&);
	void setAt(int i, const std::string&);
	void setAt(int i, const boost::python::tuple&);

	void insertAt(int i, const ParamModule&);
	void insertAt(int i, const AxialTree&);
	void insertAt(int i, const std::string&);
	void insertAt(int i, const boost::python::list&);
	void insertAt(int i, const boost::python::tuple&);

	void insertAt(iterator pos,
				  const_iterator beg,
				  const_iterator end);

	void push_back(const_iterator beg,
				   const_iterator end);

	void push_back(const_iterator pos);

	void push_front(const_iterator beg,
				   const_iterator end);

	void push_front(const_iterator pos);

	void remove(int i);
	void removeRange(int i, int j);

	inline bool empty() const { return __conststring().empty(); }
	inline size_t size() const { return __conststring().size(); }
	void clear();

	std::string str() const;
	std::string repr() const;

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

	inline const_iterator find(const ParamModule& module) const
	{ return find(module.name(),module.argSize(),const_begin(),const_end()); }

	inline const_iterator find(const ParamModule& module, 
					    const_iterator start) const
	{ return find(module.name(),module.argSize(),start,const_end()); }

	inline const_iterator find(const ParamModule& module, 
					    const_iterator start,
						const_iterator stop) const
	{ return find(module.name(),module.argSize(),start,stop); }

	inline const_iterator find(const AxialTree& modules) const
	{ return find(modules,begin(),const_end()); }

	inline const_iterator find(const AxialTree& modules, 
					    const_iterator start) const
	{ return find(modules,start,const_end()); }

	const_iterator find(const AxialTree& modules, 
					    const_iterator start,
						const_iterator stop) const;

	bool match(const ParamModule& a, 
			   const_iterator pos) const;

	bool match(const AxialTree& a, 
			   const_iterator  pos) const;

	bool match(const AxialTree& a, 
			   const_iterator  pos,
			   const_iterator& resultingpos,
			   boost::python::list& params) const;

	bool match(const AxialTree& a, 
			   const_iterator  pos,
			   const_iterator& resultingpos) const;

	bool reverse_match(const AxialTree& a, 
			   const_iterator  pos) const;

	bool reverse_match(const AxialTree& a, 
			   const_iterator  pos,
			   const_iterator& resultingpos,
			   boost::python::list& params) const;

	bool reverse_match(const AxialTree& a, 
			   const_iterator  pos,
			   const_iterator& resultingpos) const;

	bool rightmatch(const AxialTree& a, 
					const_iterator pos,
					const_iterator& resultingpos,
					boost::python::list& params) const;

	bool rightmatch(const AxialTree& a, 
					const_iterator pos,
					const_iterator& resultingpos) const;

	bool rightmatch(const AxialTree& a, 
					const_iterator pos) const;

	bool leftmatch(const AxialTree& a,  
					const_iterator pos,
					const_iterator& resultingpos,
					boost::python::list& params) const;

	bool leftmatch(const AxialTree& a,  
					const_iterator pos,
					const_iterator& resultingpos) const;

	bool leftmatch(const AxialTree& a,  
					const_iterator pos) const;

	const_iterator rightfind(const AxialTree& a,
							 const_iterator start,
						     const_iterator stop) const;

	inline const_iterator rightfind(const AxialTree& a,
							 const_iterator start) const
	{ return rightfind(a,start,const_end()); }

	inline const_iterator rightfind(const AxialTree& a) const
	{ return rightfind(a,const_begin(),const_end()); }

	const_iterator leftfind(const AxialTree& a,
							 const_iterator start,
						     const_iterator stop) const;

	inline const_iterator leftfind(const AxialTree& a,
							 const_iterator start) const
	{ return leftfind(a,start,const_end()); }

	inline const_iterator leftfind(const AxialTree& a) const
	{ return leftfind(a,const_begin(),const_end()); }

	AxialTree replace(const ParamModule&, const ParamModule&) const;
	AxialTree replace(const ParamModule&, const AxialTree&) const;
	AxialTree replace(const AxialTree&, const AxialTree&) const;

	std::vector<const_iterator> roots() const;

	const_iterator father(const_iterator pos) const;
	std::vector<const_iterator> sons(const_iterator pos) const;
	std::vector<const_iterator> lateralSons(const_iterator pos) const;
	const_iterator directSon(const_iterator pos) const;

    //!  Return iterator on endBracket ']' or end of string. If pos is on a '[', startingBeforePos allows to say if search should start from just before the '[' or after.
	const_iterator endBracket(const_iterator pos, bool startingBeforePos = false) const;
	iterator endBracket(iterator pos, bool startingBeforePos = false) const;
    //!  Return iterator on beginBracket '[' or begin of string. If pos is on a ']', startingAfterPos allows to say if search should start from just after the ']' or after.
	const_iterator beginBracket(const_iterator pos, bool startingAfterPos = false) const;

	bool wellBracketed() const;
	bool isAPath() const;

	bool hasQueryModule() const;

	inline size_t pos(const_iterator it) const
	{ return std::distance(const_begin(),it);  }

    bool operator==(const AxialTree&) const;
    inline bool operator!=(const AxialTree& other) const { return !operator==(other); }

private:

 struct AxialTreeInternal 
#ifdef USE_SHARED_DATA 
     : public QSharedData 
#endif
 {
     AxialTreeInternal() ;
     AxialTreeInternal(const AxialTreeInternal& other);	
     AxialTreeInternal(const ParamModule&);
     AxialTreeInternal(AxialTree::ModuleList::const_iterator beg, AxialTree::ModuleList::const_iterator end);
     ~AxialTreeInternal();

     AxialTree::ModuleList __string;
 };

#ifdef USE_SHARED_DATA 
 typedef QSharedDataPointer<AxialTreeInternal> AxialTreeInternalPtr;

 AxialTreeInternalPtr __data;
#else
 AxialTreeInternal __data;
#endif

protected:
 const ModuleList& __conststring() const; 
 const ModuleList& __string() const; 
 ModuleList& __string() ;
  

};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
