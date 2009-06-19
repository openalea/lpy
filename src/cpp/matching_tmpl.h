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

#ifndef __matching_tpl_h__
#define __matching_tpl_h__

#include "matching.h"
#include "axialtree_iter.h"
#include "argcollector_core.h"
#include <boost/python.hpp>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

template<class argtype, class PIterator, class Iterator>
void process_get_iterator(PIterator pattern, 
						  Iterator it, 
						  Iterator string_end, 
						  argtype& params){
	if(pattern->argSize() == 0) LsysWarning("?I should have at least one argument to hold iterator");
	PyAxialTreeIterator pyiter(it,string_end);
	if(pattern->argSize() > 1) { 
		LsysWarning("?I have too much arguments. Shoud be only one");
		argtype lp;
		ArgsCollector::append_arg(lp,bp::object(pyiter));
		for (size_t i = 1; i < pattern->argSize(); ++i) 
			ArgsCollector::append_arg(lp,bp::object());
		ArgsCollector::append_args(params,lp);
	}
	else ArgsCollector::append_arg(params,bp::object(pyiter));

}


template<class argtype, class PIterator, class Iterator>
bool process_get_module(PIterator pattern, 
						Iterator it, 
						argtype& params)
{
	argtype lp;
	ParamModule lpattern = bp::extract<ParamModule>(pattern->getAt(1))();
	if (lpattern.argSize() > 0){
		if(MatchingEngine::module_match(*it,lpattern,lp)){
			ArgsCollector::append_arg(params,bp::object(*it));
			ArgsCollector::append_args(params,lp); 
		}
		else return false;
	}
	else { 
		if(it->getClass() == lpattern.getClass())
			ArgsCollector::append_arg(params,bp::object(*it));
		else return false;
	}
	return true;
}


/*---------------------------------------------------------------------------*/

template<class Iterator,class RPIterator>
struct GetFather {
public:
	static inline Iterator next(Iterator pos, RPIterator pattern, 
							    Iterator string_begin, Iterator string_end) { 
		return father(pos,string_begin,string_end);
	}
};

template<class Iterator,class RPIterator>
struct GetScalePredecessor {
public:
	static inline Iterator next(Iterator pos, RPIterator pattern, 
		                        Iterator string_begin, Iterator string_end) { 
		return predecessor_at_scale(pos,pattern->scale(),string_begin,string_end);
	}
};

template<class Iterator,class RPIterator>
struct GetLevelPredecessor {
public:
	static inline Iterator next(Iterator pos, RPIterator pattern, 
		                        Iterator string_begin, Iterator string_end) { 
		return predecessor_at_level(pos,pattern->scale(),string_begin,string_end);
	}
};

template<class Iterator,class PIterator>
struct StringNext {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end) { 
		return pos+1;
	}

};

template<class Iterator,class PIterator>
struct GetNext {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end) { 
		return next_module(pos,string_end);
	}
	static inline Iterator initial_next(Iterator pos, PIterator pattern, Iterator last_matched, Iterator string_end) { 
		return next_module(pos,string_end,true);
	}
};

template<class Iterator,class PIterator>
struct StringPrevious {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_begin, Iterator string_end) { 
		if (pos == string_begin) return string_end;
		return pos-1;
	}
};

template<class Iterator,class PIterator>
struct GetPrevious {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_begin, Iterator string_end) { 
		return previous_module(pos,string_begin,string_end);
	}
};

template<class Iterator,class PIterator>
struct GetScaleSuccessor {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end) { 
		return next_module_at_scale(pos,pattern->scale(),string_end);
	}

	static inline Iterator initial_next(Iterator pos, PIterator pattern, Iterator last_matched, Iterator string_end) { 
		return next_module_at_scale(pos,pattern->scale(),string_end,true, last_matched->scale());
	}
};

template<class Iterator, class PIterator>
struct GetLevelSuccessor {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end) { 
		return next_module_at_level(pos,pattern->scale(),string_end);
	}
	static inline Iterator initial_next(Iterator pos, PIterator pattern, Iterator last_matched, Iterator string_end) { 
		return next_module_at_level(pos,pattern->scale(),string_end,true);
	}
};

/*---------------------------------------------------------------------------*/

template<class Matcher>
struct RegExpMatcher {
public:
	typedef typename Matcher::Iterator Iterator;
	typedef typename Matcher::PIterator PIterator;
	typedef typename Matcher::argtype argtype;

	static bool match(Iterator matching_start, Iterator  string_end,
					  PIterator pattern, Iterator& last_matched,  
					  Iterator& matching_end, 
					  argtype& lparams){
		Iterator it = matching_start;
		PIterator it2 = pattern;
		if(it2->isRepExp()){
			std::vector<argtype> llp;
			const AxialTree& lpattern = bp::extract<const AxialTree&>(it2->getAt(0))();
			size_t miniter = 0;
			if (it2->argSize() > 1) miniter = bp::extract<size_t>(it2->getAt(1))();
			size_t maxiter = 1000;
			if (it2->argSize() == 3) maxiter = bp::extract<size_t>(it2->getAt(2))();
			else if (it2->argSize() == 2) maxiter = miniter;
			bool ok = true;
			size_t numiter = 0;
			while(ok && numiter < maxiter) {
				argtype lp;
				if((ok = Matcher::match(it,string_end,lpattern.begin(),lpattern.end(),last_matched,it,lp)))
				{  llp.push_back(lp); last_matched = it; ++it; ++numiter; }
			}
			if (numiter < miniter) return false;
			--it;
			if(numiter == 0){
				size_t nbvar = lpattern.getVarNb();
				for(size_t i = 0; i < nbvar; ++i){
					ArgsCollector::append_arg(lparams,bp::list());
				}
			}
			else { ArgsCollector::append_args(lparams,ArgsCollector::fusion_args(llp)); }
		}
		else if(it2->isOr()){
			int matched = -1;			
			argtype lp;
			std::vector<size_t> nbargs;
			for(int ip = 0;ip < it2->argSize(); ++ip){
				AxialTree lpattern = bp::extract<AxialTree>(it2->getAt(ip))();
				nbargs.push_back(lpattern.getVarNb());
				if(matched == -1) { 
					if(Matcher::match(it,string_end,lpattern.begin(),lpattern.end(),last_matched,it,lp)) matched = ip;
				}
			}
			if (matched == -1) return false;
			else {
				size_t nbNone = 0;
				for(int ip = 0;ip < matched; ++ip)nbNone += nbargs[ip];
				ArgsCollector::append_n_arg(lp,nbNone,bp::object());
				nbNone = 0;
				for(int ip = matched+1;ip < it2->argSize(); ++ip)nbNone += nbargs[ip];
				ArgsCollector::append_n_arg(lp,nbNone,bp::object());
				ArgsCollector::append_args(lparams,lp); 
			}
		}
		matching_end = it;
		return true;
	}
};

/*---------------------------------------------------------------------------*/

template<
template < typename, typename> class _NextElement = StringNext, 
class _Iterator = AxialTree::const_iterator, 
class _PIterator = AxialTree::const_iterator,
class _argtype = ArgList>
struct StringMatcher
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
	typedef _PIterator PIterator;
	typedef _NextElement<Iterator,PIterator> Next;
	typedef StringMatcher<_NextElement,_Iterator,_PIterator,_argtype> MType;

	static bool match(Iterator matching_start, Iterator  string_end,
					  PIterator pattern_begin, PIterator  pattern_end, 
					  Iterator& matching_end, Iterator& last_matched,
					  argtype& params)
	{

		Iterator it = matching_start;
		Iterator pit = it;
		argtype lp;
		for (PIterator it2 = pattern_begin; it2 != pattern_end; ++it2){
			argtype lmp;
			if( it == string_end) return false;
			if(it2->isGetModule()){ if(!process_get_module(it2,it,lp)) return false; }
			else if( it2->isRE() ) { if(!RegExpMatcher<MType>::match(it,string_end,it2,pit,it,lp))return false; }
			else { 
				if( !MatchingEngine::module_match(*it,*it2,lmp))return false;
			    else ArgsCollector::append_args(lp,lmp); 
			}
			pit = it;
			it = Next::next(it,it2,string_end); 
		}
		params = lp;
		matching_end = it;
		last_matched = pit;
		return true;
	}
};

/*---------------------------------------------------------------------------*/

template<
template < typename, typename> class PreviousElement = StringPrevious, 
class _Iterator = AxialTree::const_iterator, 
class _PRIterator = AxialTree::const_reverse_iterator,
class _argtype = ArgList>
struct StringReverseMatcher 
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
	typedef _PRIterator PRIterator;
	typedef PreviousElement<Iterator,PRIterator> Previous;
	typedef StringReverseMatcher<PreviousElement,_Iterator,_PRIterator,_argtype> MType;

	static bool match(Iterator matching_start, Iterator  string_begin, Iterator  string_end,
					  PRIterator pattern_rbegin, PRIterator  pattern_rend, 
					  Iterator& matching_end, argtype& params)
	{
		Iterator it = matching_start;
		argtype lp;
		for (PRIterator it2 = pattern_rbegin; it2 != pattern_rend; ){
			argtype lmp;
			if(it2->isGetModule()){ if(!process_get_module(it2,it,lmp)) return false; }
			else if(!MatchingEngine::module_match(*it,*it2,lmp)) return false; 
			++it2;
			if (it == string_begin){ if (it2 != pattern_rend) return false; }
			else it = Previous::next(it,it2,string_begin,string_end);
			ArgsCollector::prepend_args(lp,lmp);
		}
		params = lp;
		matching_end = it;
		return true; 
	}
};

/*---------------------------------------------------------------------------*/

template<
template < typename, typename > class FatherElement = GetFather, 
class _Iterator = AxialTree::const_iterator, 
class _PRIterator = AxialTree::const_reverse_iterator,
class _argtype = ArgList>
struct TreeLeftMatcher 
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
	typedef _PRIterator PRIterator;
	typedef FatherElement<Iterator,PRIterator> Father;
	typedef TreeLeftMatcher<FatherElement,_Iterator,_PRIterator,_argtype> MType;

	static bool match(Iterator matching_start, Iterator  string_begin, Iterator  string_end,
		PRIterator pattern_rbegin, PRIterator  pattern_rend, Iterator& matching_end,
		argtype& params)
	{
		Iterator it = matching_start;
		if(it == string_begin)return false;
		PRIterator it2 = pattern_rbegin;
		it = Father::next(it,it2,string_begin,string_end);
		argtype lparams;
		while(it2 != pattern_rend && it != string_end){
			argtype lp;
			if(it2->isGetModule()){
				if(!process_get_module(it2,it,lp)) return false;
			}
			else if (! MatchingEngine::module_match(*it,*it2,lp) ) return false;
			ArgsCollector::prepend_args(lparams,lp);
			++it2; 
			if (it2 == pattern_rend) break;
			if(it2->isStar() || it2->isBracket()){ 
				++it2; --it; 
				while(it->isIgnored()) {
					if (it != string_begin) --it;
					else return false;
				}
			}
			else it = Father::next(it,it2,string_begin,string_end);
		}
		if((it2 == pattern_rend)){
			matching_end = it;
			ArgsCollector::prepend_args(params,lparams);
			return true;
		}
		else return false;
	}
};
/*---------------------------------------------------------------------------*/

template<
template < typename, typename > class _NextElement = GetNext,
class _Iterator = AxialTree::const_iterator, 
class _PIterator = AxialTree::const_iterator,
class _argtype = ArgList>
struct TreeRightMatcher 
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
	typedef _PIterator PIterator;
	typedef _NextElement<Iterator,PIterator> NextElement;
	typedef TreeRightMatcher<_NextElement,Iterator,PIterator,argtype> MType;

	static bool match(Iterator matching_start, Iterator  string_end,
					  PIterator pattern_begin, PIterator  pattern_end, 
					  Iterator last_matched, Iterator& matching_end, 
					  argtype& params)
	{
		Iterator it = matching_start;
		PIterator it2 = pattern_begin;
		argtype lparams;
		it = NextElement::initial_next(it,it2,last_matched,string_end);				
		bool nextpattern = true;
		bool nextsrc = true;
		while(it != string_end && it2 != pattern_end){
			nextpattern = true;
			nextsrc = true;
			if(it2->isStar()){
				argtype lp;
				if(MatchingEngine::module_match(*it,*it2,lp)){ 
					ArgsCollector::append_args(lparams,lp);
				}
				else return false;
			}
			else if(it2->isGetIterator()){
				process_get_iterator(it2,it,string_end,lparams);
				nextsrc = false;
			}
			else if(it2->isGetModule()){
				if(!process_get_module(it2,it,lparams)) return false;
			}
			else if(it2->isRE()) {
				if(!RegExpMatcher<MType>::match(it,string_end,it2,last_matched,it,lparams)) return false;
			}
			else if(!it2->isBracket()){ // matching a pattern module
				if(!it->isBracket()) {
					argtype lp; // if not a bracket, try to match
					if(MatchingEngine::module_match(*it,*it2,lp)){
						ArgsCollector::append_args(lparams,lp); 
					}
					else return false;
				}
				else { // it->isBracket()
					// if bracket, skip or return false
					if(it->isRightBracket())return false; 
					else if(it->isLeftBracket()) {
						it = endBracket(it,string_end);
						nextpattern = false;
					}
				}
			}
			else { // it2->isBracket()
				if(it2->isRightBracket()){
					if(!it->isRightBracket()) {
						if(it2->isExactRightBracket())return false;
						else {
							// search start before it to avoid matching A[B]C with A[B[]C]C
							it = endBracket(it,string_end,true);
						}
					}
				}
				else { // it2->isLeftBracket()
					if(!it->isLeftBracket())return false;
				}
			}
			if (nextpattern) ++it2;
			if (nextsrc && it!=string_end && it2 != pattern_end) {
				last_matched = it;
				it = NextElement::next(it,it2,string_end);				
			}
		}
		if(it2 != pattern_end && it2->isGetIterator()){
			process_get_iterator(it2,it,string_end,lparams);
			++it2;
		}
		if(it2 == pattern_end){
			matching_end = it;
			params = lparams;
			return true;
		}
		else return false;
	}
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
