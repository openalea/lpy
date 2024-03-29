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
#include "nodemodule.h"
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
						Iterator string_beg, 
						Iterator string_end, 
						argtype& params,
                        const ConsiderFilterPtr& filter)
{
	argtype lp;
	// get arg 1. It is supposed to be a module class that we look for
	ModuleClassPtr lpattern = bp::extract<ModuleClassPtr>(pattern->getAt(1).getPyValue())();
	/* if (lpattern.argSize() > 0){
		if(MatchingEngine::module_match(*it,lpattern,lp)){
			ArgsCollector::append_arg(params,bp::object(*it));
			ArgsCollector::append_args(params,lp); 
		}
		else return false;
	}
	else { */
	if(MatchingEngine::compatible_classes(it->getClass(),lpattern)) {
		// append a copy of the module
		ArgsCollector::append_arg(params,bp::object(NodeModule(it,string_beg,string_end, filter)));
    }
	else return false;
	/* }*/
	return true;
}


/*---------------------------------------------------------------------------*/

template<class Iterator,class RPIterator, class IteratorMap>
struct GetFather {
public:
	static inline Iterator next(Iterator pos, RPIterator pattern, 
							    Iterator string_begin, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return parent(pos, string_begin, string_end, filter, iteratormap);
	}
};

template<class Iterator,class RPIterator, class IteratorMap>
struct GetScalePredecessor {
public:
	static inline Iterator next(Iterator pos, RPIterator pattern, 
		                        Iterator string_begin, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return predecessor_at_scale(pos, pattern->scale(), string_begin, string_end, filter, iteratormap);
	}
};

template<class Iterator,class RPIterator, class IteratorMap>
struct GetLevelPredecessor {
public:
	static inline Iterator next(Iterator pos, RPIterator pattern, 
		                        Iterator string_begin, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return predecessor_at_level(pos, pattern->scale(), string_begin, string_end, filter, iteratormap);
	}
};

template<class Iterator,class PIterator, class IteratorMap>
struct StringNext {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return pos+1;
	}

};

template<class Iterator,class PIterator, class IteratorMap>
struct GetNext {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return next_module(pos, string_end, false, filter);
	}
	static inline Iterator initial_next(Iterator pos, PIterator pattern, Iterator last_matched, Iterator string_end, 
                                        const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return next_module(pos, string_end, true, filter);
	}
};

template<class Iterator,class PIterator, class IteratorMap>
struct StringPrevious {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_begin, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		if (pos == string_begin) return string_end;
		return pos-1;
	}
};

template<class Iterator,class PIterator, class IteratorMap>
struct GetPrevious {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_begin, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return previous_module(pos,string_begin,string_end,false,filter);
	}
};

template<class Iterator,class PIterator, class IteratorMap>
struct GetScaleSuccessor {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return next_module_at_scale(pos,pattern->scale(),string_end, false, -1, filter);
        // The -1 is a problem. Here we do not know the scale of the previous matched element.
	}

	static inline Iterator initial_next(Iterator pos, PIterator pattern, Iterator last_matched, Iterator string_end, 
                                        const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return next_module_at_scale(pos,pattern->scale(),string_end, true, last_matched->scale(), filter);
	}
};

template<class Iterator, class PIterator, class IteratorMap>
struct GetLevelSuccessor {
public:
	static inline Iterator next(Iterator pos, PIterator pattern, Iterator string_end, 
                                const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return next_module_at_level(pos, pattern->scale(), string_end, false, filter);
	}
	static inline Iterator initial_next(Iterator pos, PIterator pattern, Iterator last_matched, Iterator string_end, 
                                        const ConsiderFilterPtr& filter, IteratorMap* iteratormap = NULL) { 
		return next_module_at_level(pos, pattern->scale(), string_end, true, filter);
	}
};

/*---------------------------------------------------------------------------*/

template<class Matcher>
struct RegExpMatcher {
public:
	typedef typename Matcher::Iterator Iterator;
    typedef typename Matcher::PIterator PIterator;
    typedef typename Matcher::IteratorMap IteratorMap;
	typedef typename Matcher::argtype argtype;

	static bool match(Iterator matching_start, Iterator  string_beg, Iterator  string_end,
					  PIterator pattern, Iterator& last_matched,  
					  Iterator& matching_end, const ConsiderFilterPtr& filter, 
					  argtype& lparams, IteratorMap* iteratormap = NULL ){
		Iterator it = matching_start;
		PIterator it2 = pattern;
		if(it2->isRepExp()){
			std::vector<argtype> llp;
			const PatternString& lpattern = bp::extract<const PatternString&>(it2->getAt(0).getPyValue())();
			size_t miniter = 0;
			if (it2->argSize() > 1) miniter = bp::extract<size_t>(it2->getAt(1).getPyValue())();
			size_t maxiter = 1000;
			if (it2->argSize() == 3) maxiter = bp::extract<size_t>(it2->getAt(2).getPyValue())();
			else if (it2->argSize() == 2) maxiter = miniter;
			bool ok = true;
			size_t numiter = 0;
			while(ok && numiter < maxiter) {
				argtype lp;
				if((ok = Matcher::match(it,string_beg,string_end,lpattern.begin(),lpattern.end(),last_matched,it,filter,lp, iteratormap)))
				{  llp.push_back(lp); last_matched = it; ++it; ++numiter; }
			}
			if (numiter < miniter) {
                return false;
            }
            // if (it != string_beg) --it;
            if(numiter > 0){
                ArgsCollector::append_args(lparams,ArgsCollector::fusion_args(llp)); 
            }
			else {
				size_t nbvar = lpattern.getVarNb();
				for(size_t i = 0; i < nbvar; ++i){
					ArgsCollector::append_arg(lparams,bp::list());
				}
			}
		}
		else if(it2->isOr()){
 			int matched = -1;			
			argtype lp;
			std::vector<size_t> nbargs;
			for(int ip = 0;ip < it2->argSize(); ++ip){
				const PatternString& lpattern = bp::extract<const PatternString&>(it2->getAt(ip).getPyValue())();
				nbargs.push_back(lpattern.getVarNb());
				if(matched == -1) { 
					if(Matcher::match(it,string_beg,string_end,lpattern.begin(),lpattern.end(),last_matched,it,filter,lp, iteratormap)) {
                        matched = ip;
                    }
				}
			}
			if (matched == -1) return false;
			else {
				size_t nbNone = 0;
				for(int ip = 0;ip < matched; ++ip)nbNone += nbargs[ip];
				ArgsCollector::prepend_n_arg(lp,nbNone,bp::object());
				nbNone = 0;
				for(int ip = matched+1;ip < it2->argSize(); ++ip)nbNone += nbargs[ip];
				ArgsCollector::append_n_arg(lp,nbNone,bp::object());
				ArgsCollector::append_args(lparams,lp); 
			}
		}
		matching_end = it;
		return true;
	}

	static bool reverse_match(Iterator matching_start, Iterator  string_begin, Iterator  string_end,
					  PIterator pattern, Iterator& matching_end, const ConsiderFilterPtr& filter, argtype& lparams, IteratorMap* iteratormap = NULL)
	{
		Iterator it = matching_start;
		PIterator it2 = pattern;
		if(it2->isRepExp()){
			std::vector<argtype> llp;
			const PatternString& lpattern = bp::extract<const PatternString&>(it2->getAt(0).getPyValue())();
			size_t miniter = 0;
			if (it2->argSize() > 1) miniter = bp::extract<size_t>(it2->getAt(1).getPyValue())();
			size_t maxiter = 1000;
			if (it2->argSize() == 3) maxiter = bp::extract<size_t>(it2->getAt(2).getPyValue())();
			else if (it2->argSize() == 2) maxiter = miniter;
			bool ok = true;
			size_t numiter = 0;
			while(ok && numiter < maxiter) {
				argtype lp;
				if((ok = Matcher::match(it,string_begin,string_end,lpattern.rbegin(),lpattern.rend(),it,filter,lp,iteratormap)))
				{  llp.insert(llp.begin(),lp); ++numiter; }
				// printf("%s %i %s\n",it->name().c_str(),numiter,(ok?"Ok":"Stop"));
			}
			if (numiter < miniter) return false;
			// --it;
			if(numiter == 0){
				size_t nbvar = lpattern.getVarNb();
				for(size_t i = 0; i < nbvar; ++i){
					ArgsCollector::prepend_arg(lparams,bp::list());
				}
			}
			else { ArgsCollector::prepend_args(lparams,ArgsCollector::fusion_args(llp)); }
		}
		else if(it2->isOr()){
			int matched = -1;			
			argtype lp;
			std::vector<size_t> nbargs;
			for(int ip = 0;ip < it2->argSize(); ++ip){
				PatternString lpattern = bp::extract<PatternString>(it2->getAt(ip).getPyValue())();
				nbargs.push_back(lpattern.getVarNb());
				if(matched == -1) { 
					if(Matcher::match(it,string_begin,string_end,lpattern.rbegin(),lpattern.rend(),it,filter,lp,iteratormap))  {
                        matched = ip;
                    }
				}
			}
			if (matched == -1) return false;
			else {
				size_t nbNone = 0;
				for(int ip = 0;ip < matched; ++ip)nbNone += nbargs[ip];
				ArgsCollector::prepend_n_arg(lp,nbNone,bp::object());
				nbNone = 0;
				for(int ip = matched+1;ip < it2->argSize(); ++ip)nbNone += nbargs[ip];
				ArgsCollector::append_n_arg(lp,nbNone,bp::object());
				ArgsCollector::prepend_args(lparams,lp); 
			}
		}
		matching_end = it;
		return true;
	}
};

/*---------------------------------------------------------------------------*/

template<
template < typename, typename, typename> class _NextElement = StringNext, 
class _Iterator = AxialTree::const_iterator, 
class _PIterator = PatternString::const_iterator,
class _IteratorMap = AxialTree::IteratorMap,
class _argtype = ArgList>
struct StringMatcher
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
    typedef _PIterator PIterator;
    typedef _IteratorMap IteratorMap;
	typedef _NextElement<Iterator,PIterator,IteratorMap> Next;
	typedef StringMatcher<_NextElement,Iterator,PIterator,IteratorMap,argtype> MType;

	static bool match(Iterator matching_start, Iterator  string_beg, Iterator  string_end,
					  PIterator pattern_begin, PIterator  pattern_end, 
					  Iterator& matching_end, Iterator& last_matched, const ConsiderFilterPtr& filter,
					  argtype& params, IteratorMap* iteratormap = NULL)
	{

		Iterator it = matching_start;
		Iterator pit = it;
		argtype lp;
		for (PIterator it2 = pattern_begin; it2 != pattern_end; ++it2){
			// printf("string[%i]='%s' matched with exp[%i]='%s'\n", distance(string_beg,it), it->str().c_str() , distance(pattern_begin,it2), it2->str().c_str() );
			argtype lmp;
			if( it == string_end) return false;
			if(it2->isGetModule()){ if(!process_get_module(it2,it,string_beg,string_end,lp, filter)) return false; }
			else if( it2->isRE() ) { if(!RegExpMatcher<MType>::match(it,string_beg,string_end,it2,pit,it,filter,lp,iteratormap))return false; }
			else { 
				if( !MatchingEngine::module_match(*it,*it2,lmp)) return false;
			    else ArgsCollector::append_args(lp,lmp); 
			}
			pit = it;
			it = Next::next(it,it2,string_end,filter,iteratormap); 
		}
		params = lp;
		matching_end = it;
		last_matched = pit;
		return true;
	}
};

/*---------------------------------------------------------------------------*/

template<
template < typename, typename, typename> class PreviousElement = StringPrevious, 
class _Iterator = AxialTree::const_iterator, 
class _PRIterator = PatternString::const_reverse_iterator,
class _IteratorMap = AxialTree::IteratorMap,
class _argtype = ArgList>
struct StringReverseMatcher 
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
	typedef _PRIterator PIterator;
    typedef _IteratorMap IteratorMap;
	typedef PreviousElement<Iterator,PIterator,IteratorMap> Previous;
	typedef StringReverseMatcher<PreviousElement,Iterator,PIterator,IteratorMap,argtype> MType;

	static bool match(Iterator matching_start, Iterator  string_begin, Iterator  string_end,
					  PIterator pattern_rbegin, PIterator  pattern_rend, 
					  Iterator& matching_end, const ConsiderFilterPtr& filter, argtype& params, IteratorMap* iteratormap = NULL)
	{
        // printf("start reverse  match [%i] %s\n", distance(string_begin,matching_start), matching_start->str().c_str() );
		/* matching_start is supposed to be on the first element to test.
		   matching_end will be on the first not matched element */
		Iterator it = matching_start;
        PIterator it2 = pattern_rbegin;
        argtype lp;

        if (it == string_end) {
            // We test here if we can match nothing with 'None' or some regexp
            size_t d = distance( pattern_rbegin, pattern_rend);
            if (d == 0) return true;
            argtype lp;
            while (it2 != pattern_rend && it2->isNone()) ++it2;
            while (it2 != pattern_rend && it2->isRE()){
                argtype lmp;
                if(!RegExpMatcher<MType>::reverse_match(it,string_begin,string_end,it2,it,filter,lmp,iteratormap)) 
                    return false; 
                ArgsCollector::prepend_args(lp,lmp);
                ++it2;
                while (it2 != pattern_rend && it2->isNone()) ++it2;
            }
            if (it2 != pattern_rend) return false;

            params = lp;
            return true;
        }

		for (PIterator it2 = pattern_rbegin; it2 != pattern_rend; ){
			if( it2->isRE() ) { 
				if(!RegExpMatcher<MType>::reverse_match(it,string_begin,string_end,it2,it,filter,lp,iteratormap))return false; 
				++it2;
			}
			else {
				argtype lmp; 
				if(it2->isGetModule()){ if(!process_get_module(it2,it,string_begin,string_end,lmp, filter)) return false;  }
				else if(!MatchingEngine::module_match(*it,*it2,lmp)) return false; 
				ArgsCollector::prepend_args(lp,lmp);
				++it2;
				if (it2 != pattern_rend){
					it = Previous::next(it,it2,string_begin,string_end,filter,iteratormap);
					if (it == string_end) return false;
				}
				else
					// dont know what to use as reference to continue
					it = Previous::next(it,it2-1,string_begin,string_end, filter, iteratormap); 
			}
		}
		params = lp;
		matching_end = it;
		return true; 
	}
};

/*---------------------------------------------------------------------------*/

template<
template < typename, typename, typename > class FatherElement = GetFather, 
template < typename, typename, typename > class PreviousElement = GetPrevious, 
class _Iterator = AxialTree::const_iterator, 
class _PRIterator = PatternString::const_reverse_iterator,
class _IteratorMap = AxialTree::IteratorMap,
class _argtype = ArgList>
struct TreeLeftMatcher 
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
	typedef _PRIterator PIterator;
    typedef _IteratorMap IteratorMap;
	typedef FatherElement<Iterator, PIterator, IteratorMap> Father;
	typedef PreviousElement<Iterator, PIterator, IteratorMap> Previous;
	typedef TreeLeftMatcher<FatherElement,PreviousElement,Iterator,PIterator,IteratorMap,argtype> MType;


	static bool match(Iterator matching_start, Iterator  string_begin, Iterator  string_end,
		PIterator pattern_rbegin, PIterator  pattern_rend, Iterator& matching_end, const ConsiderFilterPtr& filter,
		argtype& params, IteratorMap* iteratormap = NULL)
	{
		/* matching_start is supposed to be before the first element to test.
		   matching_end will be on the last matched element */
		Iterator it = matching_start;
		PIterator it2 = pattern_rbegin;

		// In case of no left context in the string
		if (it == string_begin) {
			// We test here if we can match nothing with 'None' or some regexp
			size_t d = distance( pattern_rbegin, pattern_rend);
			if (d == 0) return true;
			argtype lp;
            while (it2 != pattern_rend && it2->isNone()) ++it2;
			while (it2 != pattern_rend && it2->isRE()){
				argtype lmp;
				if(!RegExpMatcher<MType>::reverse_match(it,string_begin,string_end,it2,it,filter,lmp,iteratormap)) 
					return false; 
				ArgsCollector::prepend_args(lp,lmp);
				++it2;
                while (it2 != pattern_rend && it2->isNone()) ++it2;
			}
			if (it2 != pattern_rend) return false;

			params = lp;
			return true;
		}

		argtype lp;
		
		for (PIterator it2 = pattern_rbegin; it2 != pattern_rend; ){
			if( it2->isRE() ) { 
				if(!RegExpMatcher<MType>::reverse_match(it,string_begin,string_end,it2,it,filter,lp, iteratormap))return false; 
				++it2;
			}
			else {
				if (it2->isLeftBracket()) it = Previous::next(it,it2,string_begin,string_end, filter,iteratormap);
				else { 
					it = Father::next(it,it2,string_begin,string_end, filter,iteratormap);
				}
				
				if (it == string_end) return false;

				argtype lmp; 
				if(it2->isGetModule()){ if(!process_get_module(it2,it,string_begin,string_end,lmp, filter)) return false;  }
				else if(it2->isGetIterator()){ 
					process_get_iterator(it2,it,string_end,lmp);
				}
				else if(!MatchingEngine::module_match(*it,*it2,lmp)) return false; 
				ArgsCollector::prepend_args(lp,lmp);
				++it2;
			}
		}
		params = lp;
		matching_end = it;
		return true; 

/*		Iterator it = matching_start;
		if(it == string_begin)return false;
		PIterator it2 = pattern_rbegin;
		it = Father::next(it,it2,string_begin,string_end);
		argtype lparams;
		while(it2 != pattern_rend && it != string_end){
			if( it2->isRE() ) { 
				if(!RegExpMatcher<MType>::reverse_match(it,string_begin,string_end,it2,it,lparams))return false; 
			}
			else {
				argtype lp;
				if(it2->isGetModule()){
					if(!process_get_module(it2,it,lp)) return false;
				}
				else if (! MatchingEngine::module_match(*it,*it2,lp) ) return false;
				ArgsCollector::prepend_args(lparams,lp);
			}
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
*/
	}
};
/*---------------------------------------------------------------------------*/

template <class Iterator>
bool is_equal(Iterator it1, Iterator it2){
#if _MSC_VER == 1500
	return &(*it1) == &(*it2);
#else
	return it1 == it2; 
#endif
}

template<
template < typename, typename, typename > class _NextElement = GetNext,
class _Iterator = AxialTree::const_iterator, 
class _PIterator = PatternString::const_iterator,
class _IteratorMap = AxialTree::IteratorMap,
class _argtype = ArgList>
struct TreeRightMatcher 
{
	typedef _argtype argtype;
	typedef _Iterator Iterator;
	typedef _PIterator PIterator;
	typedef _IteratorMap IteratorMap;
    typedef _NextElement<Iterator, PIterator, IteratorMap> NextElement;
	typedef TreeRightMatcher<_NextElement,Iterator,PIterator,IteratorMap,argtype> MType;

	static bool match(Iterator matching_start, 
                      Iterator  string_beg, Iterator  string_end,
					  PIterator pattern_begin, PIterator  pattern_end, 
					  Iterator& last_matched, Iterator& matching_end, 
                      const ConsiderFilterPtr& filter, argtype& params, 
                      IteratorMap* iteratormap = NULL)
	{
		Iterator it = matching_start;
		PIterator it2 = pattern_begin;
        Iterator _last_matched = last_matched;
		// In case of no right context in the string
		if (it == string_end) {
			// We test here if we can match nothing with 'None' or some regexp
			size_t d = distance( pattern_begin, pattern_end);
			if (d == 0) return true;
			if (d == 1 && pattern_begin->isNone()) return true;
			argtype lp;
			while (it2 != pattern_end && it2->isRE()){
				argtype lmp;
				if(!RegExpMatcher<MType>::match(string_end,string_beg,string_end,it2,_last_matched,it,filter,lmp,iteratormap)) 
					return false; 
				ArgsCollector::prepend_args(lp,lmp);
				++it2;
			}
			if (it2 != pattern_end) return false;
			params = lp;
			return true;
		}

		argtype lparams;

        if (is_equal(it,_last_matched)) {
            it = NextElement::next(it,it2,string_end, filter,iteratormap);
        }
		else { 
            it = NextElement::initial_next(it, it2, _last_matched, string_end, filter, iteratormap);
        }		
        std::stack<Iterator> bracketstack;
		bool nextpattern = true;
		bool nextsrc = true;
		while(it != string_end && it2 != pattern_end){
			nextpattern = true;
			nextsrc = true;
			if(it2->isStar()){
				/// We should take into account when scale is asked
				argtype lp;
				if(MatchingEngine::module_match(*it,*it2,lp)){ 
					ArgsCollector::append_args(lparams,lp);
				}
				else return false;
			}
			else if(it2->isGetIterator()){
				/// We should take into account when scale is asked
				process_get_iterator(it2,it,string_end,lparams);
				nextsrc = false;
			}
			else if(it2->isNone()){
				nextsrc = false;
			}
			else if(it2->isGetModule()){
				/// We should take into account when scale is asked
				if(!process_get_module(it2,it,string_beg,string_end,lparams, filter))
					return false;
			}
			else if(it2->isRE()) {
				// We do not ask for next elem, it will be made after.
                Iterator it1 = it;
				if(!RegExpMatcher<MType>::match(it,string_beg,string_end,it2,_last_matched,it,filter,lparams,iteratormap)) 
					return false;
                nextsrc = false;
			}
			else if(!it2->isBracket()){ // matching a pattern module
				/// We should take into account when scale is asked
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
						it = endBracket(it,string_end,iteratormap);
						nextpattern = false;
					}
				}
			}
			else { // it2->isBracket()
				/// We should take into account when scale is asked
				if(it2->isRightBracket()){
					if(!it->isRightBracket()) {
						if(it2->isExactRightBracket()) return false;
						else {                            
                            bool found = false;
                            Iterator begbracket = bracketstack.top(); bracketstack.pop();
                            if (iteratormap) {
                                typename IteratorMap::const_iterator res = iteratormap->find(iter_to_hashable(begbracket,string_end));
                                if (res != iteratormap->end()) {
                                    // printf("> use map %lu\n", std::distance(it, res->second));
                                    it = res->second;
                                    found = true;
                                }
                            }

                            if (!found)
							    // search start before it to avoid matching A[B]C with A[B[]C]C
							    it = endBracket(it,string_end,iteratormap, true);
						}
					}
				}
				else { // it2->isLeftBracket()
					if(!it->isLeftBracket())return false;
                    bracketstack.push(it);
				}
			}
			if (nextpattern) ++it2;
			if (nextsrc) { 
				_last_matched = it;
                if(it != string_end && it2 != pattern_end)
    				it = NextElement::next(it, it2, string_end, filter, iteratormap);				
			}
		}
		if(it2 != pattern_end && it2->isGetIterator()){
			process_get_iterator(it2,it,string_end,lparams);
			++it2;
		}
		if(it2 == pattern_end){
			matching_end = it;
			params = lparams;
            last_matched = _last_matched;
			return true;
		}
		else return false;
	}
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
