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

#ifndef __lpy_abstractlstring_h__
#define __lpy_abstractlstring_h__

#include <vector>
#include <functional>
#include "axialtree_manip.h"
#include <QtCore/QSharedData>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/
/*
LPY_API enum eDirection {
      eForward,
      eBackward
} ;*/

/*---------------------------------------------------------------------------*/

template<class Module>
class AbstractLString {
public:
  /// The type of element contained in the axialtree.
  typedef Module element_type;
  typedef Module ModuleType;

  /// The type of the module container
  typedef std::vector<element_type> ModuleList;

  /// An iterator used to iterate through an AxialTree.
  typedef typename ModuleList::iterator iterator;

  /// A const iterator used to iterate through an AxialTree.
  typedef typename ModuleList::const_iterator const_iterator;

  /// An iterator used to iterate through an AxialTree.
  typedef typename ModuleList::reverse_iterator reverse_iterator;

  /// A const iterator used to iterate through an AxialTree.
  typedef typename ModuleList::const_reverse_iterator const_reverse_iterator;

  /// The type of this Lstring
  typedef AbstractLString<Module> AbstractLStringType;

private:

 template<class AbstractLString>
 struct LSInternal  : public QSharedData 
 {
 public:
	 typedef typename AbstractLString::ModuleType ModuleType;
	 typedef typename AbstractLString::ModuleList ModuleList;
	 typedef typename ModuleList::const_iterator const_iterator;

	 LSInternal() : 
		QSharedData(), __string() {}
	 LSInternal(const LSInternal& other) : 
		QSharedData(other), __string(other.__string) {}
	 LSInternal(const ModuleType& m): 
		QSharedData(),__string(1,m) {}
     LSInternal(const_iterator beg, const_iterator end) : 
			QSharedData(), __string(beg,end) {}
     ~LSInternal() {}

     ModuleList __string;
 };


 typedef LSInternal<AbstractLString<typename AbstractLString::element_type> > AbstractLStringInternal;
 typedef QSharedDataPointer<AbstractLStringInternal> AbstractLStringInternalPtr;

 AbstractLStringInternalPtr __data;

protected:
 const ModuleList& __conststring() const { return __data->__string; }
 const ModuleList& __string() const { return __data->__string; }
 ModuleList& __string()  { return __data->__string; }
 void resetString()  { __data = AbstractLStringInternalPtr(new AbstractLStringInternal()); }
 
public:

  AbstractLString() : 
	__data(new AbstractLStringInternal()) {}

  AbstractLString(const AbstractLStringType& other) : 
	__data(other.__data) {}

  AbstractLString(const Module& mod) : 
	__data(new AbstractLStringInternal(mod)) {}

  AbstractLString(const_iterator beg, const_iterator end) : 
	__data(new AbstractLStringInternal(beg,end)) {}

  ~AbstractLString() { }
 

  /// Returns an iterator at the beginning of \e self.
  inline iterator begin() { return __string().begin(); }

  /// Returns an iterator at the beginning of \e self.
  inline const_iterator begin() const { return __conststring().begin(); }

  /// Returns an iterator at the beginning of \e self.
  inline const_iterator const_begin() const { return __conststring().begin(); }

  /// Returns an iterator at the end of \e self.
  inline iterator end() { return __string().end(); }

  /// Returns a const iterator at the end of \e self.
  inline const_iterator end() const { return __conststring().end(); }

  /// Returns a const iterator at the end of \e self.
  inline const_iterator const_end() const { return __conststring().end(); }

  inline bool isEnd(const_iterator it) const
  { return (const_end() == it); }

  inline bool isBegin(const_iterator it) const
  { return (const_begin() == it); }

  /// Returns an iterator at the beginning of \e self.
  inline reverse_iterator rbegin() { return __string().rbegin(); }

  /// Returns an iterator at the beginning of \e self.
  inline const_reverse_iterator rbegin() const { return __conststring().rbegin(); }

  /// Returns an iterator at the beginning of \e self.
  inline const_reverse_iterator const_rbegin() const { return __conststring().rbegin(); }

  /// Returns an iterator at the end of \e self.
  inline reverse_iterator rend() { return __string().rend(); }

  /// Returns a const iterator at the end of \e self.
  inline const_reverse_iterator rend() const { return __conststring().rend(); }

  /// Returns a const iterator at the end of \e self.
  inline const_reverse_iterator const_rend() const { return __conststring().rend(); }

  inline bool isReverseEnd(const_reverse_iterator it) const
  { return (const_rend() == it); }

  inline bool isReverseBegin(const_reverse_iterator it) const
  { return (const_rbegin() == it); }

  inline void append(const Module& m)
  {  __string().push_back(m); }

  template<class StringType>
  inline void append(const StringType& s)
  {  __string().insert(end(),s.const_begin(),s.const_end()); }

  inline void prepend(const Module& m)
  { __string().insert(begin(),m); }

  template<class StringType>
  inline void prepend(const StringType& s)
  { __string().insert(begin(),s.const_begin(),s.const_end()); }

  template<class StringType>
  inline AbstractLString<Module> operator+=(const StringType& s)
  {  append(s); return *this; }

  inline void mult(int i) {
	if (i < 1) { clear(); return; }
	reserve(i*size());
	const_iterator pattern_beg = const_begin();
	const_iterator pattern_end = const_end();
	for(int j = 1; j < i; j++) 
		push_back(pattern_beg,pattern_end);
  }

  inline AbstractLStringType& operator*=(int i)
  { mult(i); return *this; }

  inline const Module& operator[](size_t i) const
  { return __conststring()[i]; }

  inline Module& operator[](size_t i)
  { return __string()[i]; }

  inline const Module& getAt(size_t i) const
  {  return __conststring()[i]; }

  inline Module& getAt(size_t i)
  {  return __string()[i]; }

  inline const Module& getItemAt(int i) const
  { return __conststring()[getValidIndex(i)]; }

  template<class StringType>
  inline StringType getRange(size_t ri, size_t rj) const
  {
    const_iterator beg, end;
    getValidIterators(ri, rj, beg, end);
	//size_t ri, rj; getValidIndices(i,j,ri,rj) ; 
    // printf("%i %i\n",ri,rj);
	return StringType(beg,end);
  }

  inline void setAt(size_t i, const Module& m)
  {  __string()[i] = m; }

  inline void setItemAt(int i, const Module& m)
  {  __string()[getValidIndex(i)] = m;   }

  inline void insertItemAt(int i, const Module& m)
  {
    size_t len = size();
    if( i < 0 ) i+=len;
    if (i < 0  || i >= len) throw PythonExc_IndexError("index out of range");
    if( i == len) __string().push_back(m);
    else __string().insert(begin()+i,m);
  }

  template<class StringType>
  inline void insertItemAt(int i, const StringType& s)
  {
    size_t len = size();
    if( i < 0 ) i+=len;
    if (i < 0  || i >= len) throw PythonExc_IndexError("index out of range");
    __string().insert(begin()+i,s.const_begin(),s.const_end());
  }

  inline void insertAt(size_t i, const Module& m)
  {  __string().insert(begin()+i,m); }

  template<class StringType>
  inline void insertAt(size_t i, const StringType& m)
  {  __string().insert(begin()+i,m.const_begin(),m.const_end()); }

  inline void insertAt(iterator pos, const_iterator _beg, const_iterator _end)
  {  __string().insert(pos,_beg,_end); }

  inline void push_back(const_iterator _beg, const_iterator _end)
  {  __string().insert(end(),_beg,_end);  }

  inline void push_back(const_iterator pos)
  { __string().push_back(*pos); }

  inline void push_front(const_iterator _beg, const_iterator _end)
  {  __string().insert(begin(),_beg,_end);  }

  inline void push_front(const_iterator pos)
  {  __string().insert(begin(),pos,pos+1);  }

  inline void erase(iterator pos)
  {  __string().erase(pos);  }

  inline void removeItemAt(int i)
  { __string().erase(begin()+getValidIndex(i)); }

  inline void removeRange(int i, int j)
  {
	size_t ri, rj; getValidIndices(i,j,ri,rj) ; 
	__string().erase(begin()+ri,begin()+rj);
  }

  inline bool empty() const { return __conststring().empty(); }
  inline size_t size() const { return __conststring().size(); }
  inline void reserve(size_t s) { return __string().reserve(s); }
  inline void clear() { resetString(); }

  template<class Equal /*= std::equal_to<Module>*/ >
  size_t count(const Module& module) const {
    Equal eq;
	size_t c = 0;
	for(const_iterator _it = const_begin(); _it != const_end(); _it++)
		 if (eq(_it,module)) c++;
    return c;
  }

  template<class Equal /*= std::equal_to<Module>*/ >
  void replace(const Module& i, const Module& j) const {
    Equal eq;
	const_iterator _it = const_begin();
	while(!isEnd(_it)){
		if(eq(i,_it)) { *_it = j; }
		++_it;
	}
  }

  template<class Equal /*= std::equal_to<Module>*/ >
  void replace(const Module& i, const AbstractLStringType& j) const {
    if(j.empty()) return;
    Equal eq;
	const_iterator _it = const_begin();
	while(!isEnd(_it)){
		if(eq(i,_it)) { *_it = j[0]; insertAt(_it+1,j.const_begin()+1,j.const_end()); }
		++_it;
	}
  }

  // AbstractLStringType replace(const AbstractLStringType&, const AbstractLStringType&) const;

  inline std::vector<const_iterator> roots() const
  {  return LPY::roots(const_begin(),const_end()); }

  inline const_iterator parent(const_iterator pos) const
  { return LPY::parent(pos,const_begin(),const_end());}

  inline std::vector<const_iterator> children(const_iterator pos) const
  { return LPY::children(pos,const_end()); }

  inline std::vector<const_iterator> lateral_children(const_iterator pos) const
  { return LPY::lateral_children(pos,const_end()); }

  inline const_iterator direct_child(const_iterator pos) const
  { return LPY::direct_child(pos,const_end()); }

  inline const_iterator complex(const_iterator pos, int scale) const
  {  return LPY::complex(pos,scale,const_begin(),const_end()); }

  inline const_iterator complex(const_iterator pos) const
  { return LPY::complex(pos,up_scale(pos->scale()),const_begin(),const_end()); }

  inline std::vector<const_iterator> components(const_iterator pos) const
  { return LPY::components(pos,const_end()); }

  inline std::vector<const_iterator> components_at_scale(const_iterator pos, int scale) const
  { return LPY::components_at_scale(pos,scale,const_end()); }

  //!  Return iterator on endBracket ']' or end of string. If pos is on a '[', startingBeforePos allows to say if search should start from just before the '[' or after.
  inline const_iterator endBracket(const_iterator pos, bool startingBeforePos = false) const
  { return LPY::endBracket(pos,const_end(),startingBeforePos); }

  //!  Return iterator on beginBracket '[' or begin of string. If pos is on a ']', startingAfterPos allows to say if search should start from just after the ']' or after.
  inline const_iterator beginBracket(const_iterator pos, bool startingAfterPos = false) const
  { return LPY::beginBracket(pos,const_begin(),const_end(),startingAfterPos); }

  inline bool wellBracketed() const
  { return LPY::wellBracketed(const_begin(),const_end()); }

  bool isAPath() const
  { 
    for(const_iterator pos = const_begin(); !isEnd(pos); ++pos)
	   if(pos->isBracket()) return false;
    return true; 
  }

  bool hasRequestModule() const 
  {
    const_iterator pos = const_begin();
    while(!isEnd(pos)){
      if(pos->isRequest())  return true;
	  ++pos;
    }
    return false; 
  }

  inline size_t pos(const_iterator it) const
  { return std::distance(const_begin(),it);  }

  inline bool operator!=(const AbstractLStringType& other) const { return !operator==(other); }
  bool operator==(const AbstractLStringType& other) const
  {
    if(__data == other.__data) return true;
    else {
        const ModuleList& _str = __conststring();
        const ModuleList& _other_str = other.__conststring();
        if (_str.size() != _other_str.size()) return false;
        else {
            const_iterator it = _str.begin();
            for(const_iterator it2 = _other_str.begin();it2 != _other_str.end();++it,++it2)
                if(*it != *it2) return false;
            return true;
        }
    }
  }

  inline const Module& first() const { return __conststring().front(); }
  inline Module& first() { return __string().front(); }

  inline const Module& last() const { return __conststring().back(); }
  inline Module& last() { return __string().back(); }

protected:
	 inline size_t getValidIndex(int i) const {
		size_t s = size();
		if( i < 0 ) i += s;
		if (i < 0  || i >= s) throw PythonExc_IndexError("index out of range");
		return (size_t)i;
	 }

     inline void getValidIndices(int i, int j, size_t& resi, size_t& resj) const {
        size_t s = size();
        printf("%i, %i\n",i,j);
        if( i < 0 ) i += s;
        if( j < 0 ) j += s;
        if( j > s ) j = s;
        if (i < 0  || i >= s || j < i) throw PythonExc_IndexError("index out of range");
        resi =(size_t)i;
        resj =(size_t)j;
     }

     inline void getValidIndices(size_t& resi, size_t& resj) const {
        if (resj == std::string::npos) resj = size();
     }

	 inline void getValidIterators(int i, int j, const_iterator& resi, const_iterator& resj) const {
		size_t s = size();
		if( i < 0 ) i += s;
		if( j < 0 ) j += s;
	    if( j > s ) j = s;
		if (i < 0  || i >= s || j < i) throw PythonExc_IndexError("index out of range");
		resi =const_begin()+i;
		resj =const_begin()+j;
	 }

     inline void getValidIterators(size_t i, size_t j, const_iterator& resi, const_iterator& resj) const {
        resi =const_begin()+i;
        if (j == LONG_MAX) {
            resj = const_end();
        }
        else {
            resj =const_begin()+j;
        }
     }
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
