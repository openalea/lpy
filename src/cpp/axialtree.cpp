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

#include "mod.h"
#include "axialtree.h"
#include "lpy_parser.h"
#include "tracker.h"
#include <boost/python.hpp>
#include <iostream>


using namespace boost::python;

PYLSYS_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/


AxialTree::AxialTreeInternal::AxialTreeInternal() :
#ifdef USE_SHARED_DATA
 QSharedData(),
#endif
 __string() {}

AxialTree::AxialTreeInternal::AxialTreeInternal(const AxialTreeInternal& other) :
#ifdef USE_SHARED_DATA
    QSharedData(other),
#endif
    __string(other.__string) {}


AxialTree::AxialTreeInternal::AxialTreeInternal(const ParamModule& m):
#ifdef USE_SHARED_DATA
    QSharedData(),
#endif
    __string(1,m) {}

AxialTree::AxialTreeInternal::AxialTreeInternal(AxialTree::ModuleList::const_iterator beg, 
                                     AxialTree::ModuleList::const_iterator end):
#ifdef USE_SHARED_DATA
    QSharedData(), 
#endif
    __string(beg,end) {}

AxialTree::AxialTreeInternal::~AxialTreeInternal() {}

/*---------------------------------------------------------------------------*/

#ifdef USE_SHARED_DATA
#define init_internal_data()      __data(new AxialTreeInternal())
#define init_internal_data_with(arg) __data(new AxialTreeInternal(arg))
#define init_internal_data_with2(arg1,arg2) __data(new AxialTreeInternal(arg1,arg2))
#else
#define init_internal_data()      __data()
#define init_internal_data_with(arg) __data(arg)
#define init_internal_data_with2(arg1,arg2) __data(arg1,arg2)
#endif


AxialTree::AxialTree():
  init_internal_data()
{ IncTracker(AxialTree) }

AxialTree::AxialTree(const AxialTree& m):
  __data(m.__data)
  { IncTracker(AxialTree) }

 AxialTree::AxialTree(const std::string& s):
  init_internal_data()
{ 
  IncTracker(AxialTree)
  operator+=(s);
}

AxialTree::AxialTree(const ParamModule& m):
   init_internal_data_with(m)
{ IncTracker(AxialTree) }

AxialTree::AxialTree(const boost::python::list& l):
  init_internal_data(){
  IncTracker(AxialTree) 
  operator+=(l);
}

AxialTree::AxialTree(const boost::python::tuple& t):
  init_internal_data_with(ParamModule(t))
{
	IncTracker(AxialTree) 
}

AxialTree::AxialTree(const_iterator beg, const_iterator end):
  init_internal_data_with2(beg,end)
{
	IncTracker(AxialTree) 
}

AxialTree::~AxialTree()
{
	DecTracker(AxialTree) 
}

AxialTree& AxialTree::operator=(const AxialTree& other)
{ __data = other.__data; return *this; }

#ifdef USE_SHARED_DATA 
const AxialTree::ModuleList& AxialTree::__conststring() const 
 { return __data->__string; }

const AxialTree::ModuleList& AxialTree::__string() const 
 { return __data->__string; }

AxialTree::ModuleList& AxialTree::__string() 
 { return __data->__string; }
#else
const AxialTree::ModuleList& AxialTree::__conststring() const 
 { return __data.__string; }

const AxialTree::ModuleList& AxialTree::__string() const 
 { return __data.__string; }

AxialTree::ModuleList& AxialTree::__string() 
 { return __data.__string; }
#endif

void AxialTree::clear()
#ifdef USE_SHARED_DATA
{ __data = new AxialTreeInternal(); }
#else
{ __string().clear(); }
#endif

AxialTree 
AxialTree::QueryTree(const std::string& s){
  AxialTree a;
  std::vector<std::pair<size_t,std::string> > parsedstring = parselstring(s);
  a.__string().reserve(parsedstring.size());
  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
	  it != parsedstring.end(); ++it){
		a.__string().push_back(ParamModule::QueryModule(it->first,it->second));
  }
  return a;
}

bool AxialTree::operator==(const AxialTree& other) const
{
#ifdef USE_SHARED_DATA
    if(__data == other.__data) return true;
    else {
#endif
        const ModuleList& _str = __conststring();
        const ModuleList& _other_str = other.__conststring();
        if (_str.size() != _other_str.size()) return false;
        else {
            const_iterator it = _str.begin();
            for(const_iterator it2 = _other_str.begin();it2 != _other_str.end();++it,++it2)
                if(*it != *it2) return false;
            return true;
        }
#ifdef USE_SHARED_DATA
    }
#endif
}

AxialTree& AxialTree::operator+=(const boost::python::tuple& m){
  __string().push_back(ParamModule(m));
  return *this;
}

AxialTree& AxialTree::operator+=(const boost::python::list& m){
    object iter_obj = object( handle<>( PyObject_GetIter( m.ptr() ) ) );
    while( true )
    {
        object obj;
        try {  obj = iter_obj.attr( "next" )(); }
        catch( error_already_set ){ PyErr_Clear(); break; }
        extract<size_t> idext(obj);
		if (idext.check())
			__string().push_back(idext());
		else {
			extract<std::string> st(obj);
			if(st.check())
				operator+=(AxialTree(st()));
			else {
				extract<tuple> tu(obj);
				if(tu.check())
					__string().push_back(ParamModule(tu()));
				else {
					extract<AxialTree> ax(obj);
					if(ax.check()) operator+=(ax());
					else __string().push_back(extract<ParamModule>(obj)());
				}
			}
		}
    }
    return *this;
}

AxialTree& AxialTree::operator+=(const ParamModule& m){
  __string().push_back(m);
  return *this;
}

AxialTree& AxialTree::operator+=(const std::string& s){
  std::vector<std::pair<size_t,std::string> > parsedstring = parselstring(s);

  size_t newcapacity = size()+parsedstring.size();
  if (__conststring().capacity() < newcapacity)
  __string().reserve(newcapacity);
  for(std::vector<std::pair<size_t,std::string> >::const_iterator it = parsedstring.begin();
	  it != parsedstring.end(); ++it){
		__string().push_back(ParamModule(it->first,it->second));
  }
  return *this;
}

AxialTree& AxialTree::operator+=(const AxialTree& m){
  __string().insert(end(),m.const_begin(),m.const_end());
  return *this;
}

AxialTree AxialTree::operator+(const boost::python::tuple& m)
{ AxialTree e(*this); e+=m; return e; }

AxialTree AxialTree::operator+(const boost::python::list& m)
{ AxialTree e(*this); e+=m; return e; }

AxialTree AxialTree::operator+(const ParamModule& m)
{ AxialTree e(*this); e+=m; return e; }

AxialTree AxialTree::operator+(const std::string& m)
{ AxialTree e(*this); e+=m; return e; }

AxialTree AxialTree::operator+(const AxialTree& m)
{ AxialTree e(*this); e+=m; return e; }

AxialTree& AxialTree::prepend(const ParamModule& m)
{ __string().insert(begin(),m); return *this;}

AxialTree& AxialTree::prepend(const AxialTree& m)
{ push_front(m.begin(),m.end()); return *this;}

AxialTree& AxialTree::prepend(const std::string& m)
{ AxialTree a(m); push_front(a.const_begin(),a.const_end()); return *this;}

AxialTree& AxialTree::prepend(const boost::python::list& m)
{ AxialTree a(m); push_front(a.const_begin(),a.const_end()); return *this;}

AxialTree& AxialTree::prepend(const boost::python::tuple& m)
{ AxialTree a(m); push_front(a.const_begin(),a.const_end()); return *this;}

AxialTree 
AxialTree::operator*(int i) const{
  if (i < 1) return AxialTree();
  AxialTree e(*this);
  for(int j = 1; j < i; j++)
	e+=(*this);
  return e;
}

AxialTree& AxialTree::operator*=(int i){
  *this = *this *i;
  return *this;
}

const ParamModule& AxialTree::operator[](size_t i) const{
  return __conststring()[i];
}

const ParamModule& AxialTree::getAt(int i) const{
  if( i < 0 ) i+=size();
  if (i < 0  || i >= size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return __conststring()[i];
}

AxialTree AxialTree::getRange(int i, int j) const{
  if( i < 0 ) i+=size();
  if( j < 0 ) j+=size();
  if( j > size() ) j =size();
  if (i < 0  || i >= size() || j < 0  || j > size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  return AxialTree(const_begin()+i,const_begin()+j);
}

void AxialTree::setAt(int i, const ParamModule& m){
  if( i < 0 ) i+=size();
  if( i < 0  || i >= size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  else __string()[i] = m;
}

void AxialTree::setAt(int i, const tuple& m){
  if( i < 0 ) i+=size();
  if( i < 0  || i >= size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  else __string()[i] = ParamModule(m);
}

void AxialTree::setAt(int i, const std::string& s){
  if( i < 0 ) i+=size();
  if( i < 0  || i >= size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  else __string()[i] = ParamModule(s);
}

void AxialTree::insertAt(int i, const ParamModule& m){
  if( i < 0 ) i+=size();
  if( i < 0  || i > size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  if( i == size())
	__string().push_back(m);
  else __string().insert(begin()+i,m);
}

void AxialTree::insertAt(int i, const AxialTree& m){
  if( i < 0 ) i+=size();
  if( i < 0  || i > size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  if( i == size())
	__string().insert(end(),m.const_begin(),m.const_end());
  else __string().insert(begin()+i,m.const_begin(),m.const_end());
}

void AxialTree::insertAt(int i, const std::string& s){
  insertAt(i,AxialTree(s));
}

void AxialTree::insertAt(int i, const list& s){
  insertAt(i,AxialTree(s));
}

void AxialTree::insertAt(int i, const tuple& s){
  insertAt(i,ParamModule(s));
}

void AxialTree::insertAt(AxialTree::iterator pos,
				  AxialTree::const_iterator beg,
				  AxialTree::const_iterator end)
{ 
    __string().insert(pos,beg,end); 
}

void AxialTree::push_back(AxialTree::const_iterator _beg,
				          AxialTree::const_iterator _end)
{
    __string().insert(end(),_beg,_end); 
}

void AxialTree::push_back(AxialTree::const_iterator pos)
{ 
    __string().insert(end(),pos,pos+1); 
}

void AxialTree::push_front(AxialTree::const_iterator beg,
				          AxialTree::const_iterator end)
{ 
    __string().insert(begin(),beg,end); 
}

void AxialTree::push_front(AxialTree::const_iterator pos)
{ 
    __string().insert(begin(),pos,pos+1); 
}

void AxialTree::remove(int i){
  if( i < 0 ) i+=size();
  if( i < 0  || i >= size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  else __string().erase(begin()+i);
}

void AxialTree::removeRange(int i, int j){
  if( i < 0 ) i+=size();
  if( j < 0 ) j+=size();
  if( j > size() ) j =size();
  if( i >= size()  || j > size()){
	PyErr_SetString(PyExc_IndexError, "index out of range");
    throw_error_already_set();
  }
  if( i > j){
	size_t k = i; i = j; j = k; // swap index
  }
  __string().erase(begin()+i,begin()+j);
}

std::string AxialTree::str() const{
  std::string str;
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 str += _it->str();
  return str;
}

std::string AxialTree::repr() const{
  std::string str = "AxialTree(";
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 str += _it->repr();
  str +=')';
  return str;
}

size_t
AxialTree::count(const std::string& name) const{
  size_t c = 0;
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 if (_it->name() == name )
		   c++;
  return c;
}

size_t
AxialTree::count(const std::string& name, size_t nbparam) const{
  size_t c = 0;
  for(ModuleList::const_iterator _it = const_begin();
	  _it != const_end(); _it++)
		 if (_it->name() == name && _it->argSize() == nbparam)
		   c++;
  return c;
}

size_t
AxialTree::count(const ParamModule& module) const{
  return count(module.name(),module.argSize());
}

AxialTree::const_iterator 
AxialTree::find(const std::string& name, size_t nbparam, 
				const_iterator start, 
				const_iterator stop) const
{ 
  const_iterator _it = start;
  while(_it != stop && (_it->name() != name || _it->argSize() != nbparam))++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

AxialTree::const_iterator 
AxialTree::find(const AxialTree& modules, 
				const_iterator start,
				const_iterator stop) const
{
  const_iterator _it = start;
  while(_it != stop && !match(modules,_it))++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

AxialTree::const_iterator 
AxialTree::find(const std::string& name, 
				const_iterator start, 
				const_iterator stop) const
{ 
  const_iterator _it = start;
  while(_it != stop && _it->name() != name)++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

AxialTree 
AxialTree::replace(const ParamModule& i, const ParamModule& j) const{
  AxialTree dest;
  const_iterator _it = const_begin();
  const_iterator _it2 = _it;
  while(!isEnd(_it2)){
	while(!isEnd(_it2) && !match(i,_it2))++_it2;
	dest.push_back(_it,_it2);
	if(!isEnd(_it2)){
	  dest += j;
	  ++_it2;
	  _it = _it2;
	}
  }
  return dest;
}

AxialTree 
AxialTree::replace(const ParamModule& i, const AxialTree& j) const{
  AxialTree dest;
  const_iterator _it = const_begin();
  const_iterator _it2 = _it;
  while(!isEnd(_it2)){
	while(!isEnd(_it2) && !match(i,_it2))++_it2;
	dest.push_back(_it,_it2);
	if(!isEnd(_it2)){
	  dest += j;
	  ++_it2;
	  _it = _it2;
	}
  }
  return dest;
}

AxialTree 
AxialTree::replace(const AxialTree& i, const AxialTree& j) const{
  AxialTree dest;
  const_iterator _it = const_begin();
  const_iterator _it2 = _it;
  const_iterator _it3 = _it;
  while(!isEnd(_it2)){
	while(!isEnd(_it2) && !match(i,_it2,_it3))++_it2;
	dest.push_back(_it,_it2);
	if(!isEnd(_it2)){
	  dest += j;
	  _it2 = _it3;
	  _it = _it2;
	}
  }
  return dest;
}

std::vector<AxialTree::const_iterator> 
AxialTree::roots() const
{ 
  std::vector<const_iterator> res;
  if (empty()) return res;
  AxialTree::const_iterator i = const_begin();
  if(i->isRightBracket())return res;
  else if(i->isLeftBracket()){
	while(!isEnd(i) && (i->isBracket() || i->isIgnored())){
	  while(!isEnd(i) && i->isLeftBracket()){
		std::vector<AxialTree::const_iterator> res2 = sons(i);
		if(!res2.empty())
		  res.insert(res.end(),res2.begin(),res2.end());
		i = endBracket(i);
		if( isEnd(i)) return res;
		++i;
	  }
	  while(!isEnd(i) && !i->isBracket() && i->isIgnored())++i;
	}
	if( !isEnd(i) && !i->isRightBracket()){
	  res.push_back(i);
	}
	// std::vector<const_iterator> res2 = sons(i);
	// if(!res2.empty())res.insert(res.end(),res2.begin(),res2.end());
  }
  else if(i->isIgnored()){
	std::vector<const_iterator> res2 = sons(i);
	if(!res2.empty())res.insert(res.end(),res2.begin(),res2.end());
  }
  else res.push_back(i);
  return res;
}

AxialTree::const_iterator 
AxialTree::father(const_iterator pos) const
{
  if( isBegin(pos)) return end();
  --pos;
  while(!isEnd(pos) && !isBegin(pos) && (pos->isBracket() || pos->isIgnored())){
	while( !isBegin(pos) && (pos->isLeftBracket() || pos->isIgnored()))--pos;
	while(!isEnd(pos) && !isBegin(pos) && pos->isRightBracket()){
	  pos = beginBracket(pos);
	  if( !isEnd(pos) ) --pos;
	}
  }
  if( isEnd(pos) ) return pos;
  else if( isBegin(pos) && (pos->isLeftBracket() || pos->isIgnored())) return end();
  else return pos;
}

std::vector<AxialTree::const_iterator> 
AxialTree::sons(AxialTree::const_iterator pos) const
{ 
  std::vector<AxialTree::const_iterator> result; 
  if( isEnd(pos) || pos->isRightBracket()) return result;
  ++pos;
  while(!isEnd(pos) && (pos->isBracket() || pos->isIgnored())){
	while(!isEnd(pos) && !pos->isBracket() && pos->isIgnored())++pos;
	while(!isEnd(pos) && pos->isLeftBracket()){
	  std::vector<AxialTree::const_iterator> res = sons(pos);
	  if(!res.empty())
		result.insert(result.end(),res.begin(),res.end());
	  pos = endBracket(pos);
	  if( isEnd(pos)) return result;
	  ++pos;
	}
  }
  if( !isEnd(pos) && !pos->isRightBracket()){
	result.push_back(pos);
  }
  return result;
}

std::vector<AxialTree::const_iterator> 
AxialTree::lateralSons(AxialTree::const_iterator pos) const{
  std::vector<AxialTree::const_iterator> result; 
  if( isEnd(pos) || pos->isRightBracket()) return result;
  ++pos;
  while(!isEnd(pos) && (pos->isBracket() || pos->isIgnored())){
	while(!isEnd(pos) && !pos->isBracket() && pos->isIgnored())++pos;
	while(!isEnd(pos) && pos->isLeftBracket()){
	  std::vector<AxialTree::const_iterator> res = sons(pos);
	  result.insert(result.end(),res.begin(),res.end());
	  pos = endBracket(pos);
	  if( isEnd(pos)) return result;
	  ++pos;
	}
  }
  return result;
}

AxialTree::const_iterator 
AxialTree::directSon(const_iterator pos) const
{
  if( isEnd(pos) || pos->isRightBracket()) return end();
  ++pos;
  while(!isEnd(pos) && (pos->isBracket() || pos->isIgnored())){
	while(!isEnd(pos) && !pos->isBracket() && pos->isIgnored())++pos;
	while(!isEnd(pos) && pos->isLeftBracket()){
	  pos = endBracket(pos);
	  if( isEnd(pos)) return end();
	  ++pos;
	}
  }
  if( isEnd(pos) || pos->isRightBracket()) return end();
  else return pos;
}

AxialTree::const_iterator 
AxialTree::endBracket(AxialTree::const_iterator pos, bool startingBeforePos) const
{ 
  if(isEnd(pos))return pos;
  if(!startingBeforePos && pos->isLeftBracket())++pos;
  int bracket= 0;
  while(!isEnd(pos) && (bracket>0 || !pos->isRightBracket())){
	if(pos->isLeftBracket()) bracket++;
	else if(pos->isRightBracket()) bracket--;
	++pos;
  }
  return pos; 
}

AxialTree::iterator 
AxialTree::endBracket(AxialTree::iterator pos, bool startingBeforePos) const
{ 
  if(isEnd(pos))return pos;
  if(!startingBeforePos && pos->isLeftBracket())++pos;
  int bracket= 0;
  while(!isEnd(pos) && (bracket>0 || !pos->isRightBracket())){
	if(pos->isLeftBracket()) bracket++;
	else if(pos->isRightBracket()) bracket--;
	++pos;
  }
  return pos; 
}

AxialTree::const_iterator 
AxialTree::beginBracket(AxialTree::const_iterator pos, bool startingAfterPos) const
{ 
  if(!isEnd(pos) && pos->isLeftBracket())return pos;
  if(isBegin(pos) ) return const_end();
  if(isEnd(pos))--pos;
  else if(!startingAfterPos && pos->isRightBracket())--pos;
  int bracket= 0;
  while(!isBegin(pos) && (bracket>0 || !pos->isLeftBracket())){
	if(pos->isRightBracket()) bracket++;
	else if(pos->isLeftBracket()) bracket--;
	--pos;
  }
  if(isBegin(pos)) {
	if(bracket == 0 && pos->isLeftBracket())return pos;
	else return const_end();
  }
  return pos; 
}

bool
AxialTree::wellBracketed() const
{ 
  int bracket= 0;
  AxialTree::const_iterator pos = const_begin();
  while(!isEnd(pos) && bracket>= 0){
	if(pos->isLeftBracket()) ++bracket;
	else if(pos->isRightBracket()) --bracket;
	++pos;
  }
  return (bracket == 0); 
}

bool
AxialTree::isAPath() const
{ 
  AxialTree::const_iterator pos = const_begin();
  while(!isEnd(pos)){
	if(pos->isBracket()) return false;
	++pos;
  }
  return true; 
}

bool
AxialTree::hasQueryModule() const
{
  AxialTree::const_iterator pos = const_begin();
  while(!isEnd(pos)){
      if(pos->isQuery())  return true;
	++pos;
  }
  return false; 
}

bool AxialTree::match(const ParamModule& a, AxialTree::const_iterator it) const
{ return it->match(a); }


bool AxialTree::match(const AxialTree& a, 
					  AxialTree::const_iterator it) const
{ AxialTree::const_iterator res; return match(a,it,res); }

bool AxialTree::match(const AxialTree& a, 
					  AxialTree::const_iterator it,
					  AxialTree::const_iterator& resultingpos) const
{ 
  for (AxialTree::const_iterator it2 = a.const_begin(); it2 != a.const_end(); ++it2)
	if(isEnd(it) || !it->match(*it2))return false;
	else ++it;
  resultingpos = it;
  return true; 
}

bool AxialTree::match(const AxialTree& a, 
					  AxialTree::const_iterator it,
					  AxialTree::const_iterator& resultingpos,
					  list& params) const
{ 
  list lp;
  for (AxialTree::const_iterator it2 = a.const_begin(); it2 != a.const_end(); ++it2){
	if(isEnd(it) || !it->match(*it2,lp))return false;
	else { ++it; }
  }
  params += lp;
  resultingpos = it;
  return true; 
}

bool AxialTree::reverse_match(const AxialTree& a, 
					  AxialTree::const_iterator it) const
{ AxialTree::const_iterator res; return reverse_match(a,it,res); }

bool AxialTree::reverse_match(const AxialTree& a, 
					  AxialTree::const_iterator it,
					  AxialTree::const_iterator& resultingpos) const
{ 
 for (ModuleList::const_reverse_iterator it2 = a.__conststring().rbegin(); it2 != a.__conststring().rend(); ){
	if(!it->match(*it2))return false;
    else {
        ++it2;
        if (isBegin(it)){
            if (it2 != a.__conststring().rend())return false;
        }
        else --it;
    }
 }
 resultingpos = it;
 return true; 
}

bool AxialTree::reverse_match(const AxialTree& a, 
					  AxialTree::const_iterator it,
					  AxialTree::const_iterator& resultingpos,
					  list& params) const
{ 
  list lp;
  for (ModuleList::const_reverse_iterator it2 = a.__conststring().rbegin(); it2 != a.__conststring().rend(); ){
    list lmp;
	if(isEnd(it) || !it->match(*it2,lmp)) return false; 
    else { 
        ++it2;
        if (isBegin(it)){
			if (it2 != a.__conststring().rend()) return false;
        }
        else --it;
        lmp += lp; lp = lmp; 
    }
  }
  params += lp;
  resultingpos = it;
  return true; 
}
bool AxialTree::rightmatch(const AxialTree& a, 
						   AxialTree::const_iterator it) const
{ AxialTree::const_iterator res; return rightmatch(a,it,res); }

bool AxialTree::rightmatch(const AxialTree& a, 
						   AxialTree::const_iterator it,
						   AxialTree::const_iterator& resultingpos) const{
  if(a.empty())return true;
  const_iterator it2 = a.const_begin();
  while(!isEnd(it) && !a.isEnd(it2)){
	if(it->isIgnored()){ ++it; }
	else if(it2->isStar()){ ++it; ++it2; }
	if(it2->isStar()){
		if(it->match(*it2)){ ++it; ++it2; }
		else return false;
	}
	else if(!it2->isBracket()){
	  if(!it->isBracket()) {
		if(it->match(*it2)){ ++it; ++it2; }
		else return false;
	  }
	  else { // it->isBracket()
		if(it->isRightBracket())return false;
		else if(it->isLeftBracket()) {
		  it = endBracket(it);
		  if(!isEnd(it))++it;
		}
	  }
	}
	else { // it2->isBracket()
	  if(it2->isRightBracket()){
		if(it->isRightBracket()) { ++it; ++it2; }
		else { 
		  if(it2->isExactRightBracket())return false;
		  else {
            // search start before it to avoid matching A[B]C with A[B[]C]C
			it = endBracket(it,true);
			if(!isEnd(it))++it; 
			++it2; 
		  }
		}
	  }
	  else { // it2->isLeftBracket()
		if(!it->isLeftBracket())return false;
		else { ++it; ++it2;	}
	  }
	}
  }
  if(a.isEnd(it2)){
	resultingpos = it;
	return true;
  }
  else return false;
}

bool AxialTree::rightmatch(const AxialTree& a, 
						   AxialTree::const_iterator it,
						   AxialTree::const_iterator& resultingpos,
						   list& params) const{
  if(a.empty())return true;
  const_iterator it2 = a.const_begin();
  while(!isEnd(it) && !a.isEnd(it2)){
	if(it->isIgnored()){ ++it; }
	if(it2->isStar()){
		list lp;
		if(it->match(*it2,lp)){ params += lp; ++it; ++it2; }
		else return false;
	}
	else if(!it2->isBracket()){
	  if(!it->isBracket()) {
		list lp;
		if(it->match(*it2,lp)){ params += lp; ++it; ++it2; }
		else return false;
	  }
	  else { // it->isBracket()
		if(it->isRightBracket())return false;
		else if(it->isLeftBracket()) {
		  it = endBracket(it);
		  if(!isEnd(it))++it;
		}
	  }
	}
	else { // it2->isBracket()
	  if(it2->isRightBracket()){
		if(it->isRightBracket()) { ++it; ++it2; }
		else { 
		  if(it2->isExactRightBracket())return false;
		  else {
            // search start before it to avoid matching A[B]C with A[B[]C]C
			it = endBracket(it,true);
			if(!isEnd(it))++it; 
			++it2; 
		  }
		}
	  }
	  else { // it2->isLeftBracket()
		if(!it->isLeftBracket())return false;
		else { ++it; ++it2;	}
	  }
	}
  }
  if(a.isEnd(it2)){
	resultingpos = it;
	return true;
  }
  else return false;
}

AxialTree::const_iterator
AxialTree::rightfind(const AxialTree& a,
				 const_iterator start,
				 const_iterator stop) const {
  if(a.empty())return start;
  const_iterator _it = start;
  while(_it != stop && !rightmatch(a,_it))++_it; 
  if(_it == stop)return end(); 
  else return _it;
}

bool AxialTree::leftmatch(const AxialTree& a, 
						  AxialTree::const_iterator it2,
						  AxialTree::const_iterator& resultingpos,
						  list& params) const{
  if(a.empty())return true;
  if(isBegin(it2))return false;
  list lparams;
  const_iterator it = a.end()-1;
  it2 = father(it2);
  list lp;
  while(!a.isBegin(it) && !isEnd(it2) && it2->match(*it,lp) ){
	  lparams += lp; 
	  --it; 
	  if(it->isStar() || it->isBracket()){ 
		--it; --it2; while(!isBegin(it2)&&it2->isIgnored())--it2;}
	  else  it2 = father(it2);
	  lp = list();
  }
  if(a.isBegin(it)){
	if(!isEnd(it2) && it2->match(*it,lp)){
	  lparams += lp ;
	  resultingpos = it2;
	  params += lparams;
	  return true;
	}
	else return false;
  }
  else return false;
}

bool AxialTree::leftmatch(const AxialTree& a, 
						  AxialTree::const_iterator it2,
						  AxialTree::const_iterator& resultingpos) const{
  if(a.empty())return true;
  if(isBegin(it2))return false;
  const_iterator it = a.end()-1;
  it2 = father(it2);
  while(!a.isBegin(it) && !isBegin(it2) && !isEnd(it2) && (it2->match(*it) || it->isStar())){
	  --it; 
	  if(it->isStar() || it->isBracket()){ // Star and Bracket can match Bracket.
		--it2; while(!isBegin(it2)&&it2->isIgnored())--it2;
	  }
	  else it2 = father(it2); 
  }
  if(a.isBegin(it)){
	if(!isEnd(it2) && it2->match(*it) ){
	  resultingpos = it2;
	  return true;
	}
	else return false;
  }
  else return false;
}

bool AxialTree::leftmatch(const AxialTree& a, AxialTree::const_iterator it) const
{ AxialTree::const_iterator res; return rightmatch(a,it,res); }

AxialTree::const_iterator
AxialTree::leftfind(const AxialTree& a,
					const_iterator start,
					const_iterator stop) const {
  if(a.empty())return stop;
  const_iterator _it = stop;
  bool b = false;
  while(_it != start && !(b = leftmatch(a,_it)))++_it; 
  if(_it == start && !b)return end(); 
  else return _it;
}


PYLSYS_END_NAMESPACE
