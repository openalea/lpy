#ifndef __matching_tpl_h__
#define __matching_tpl_h__

#include "matching.h"
#include "axialtree_iter.h"
#include <boost/python.hpp>

LPY_BEGIN_NAMESPACE
#define bp boost::python
/*---------------------------------------------------------------------------*/

struct BPListManager {
public:
	typedef bp::list element_type;
	static inline void append_args(element_type& value, const element_type& elements){
		value += elements;
	}

	static inline void prepend_args(element_type& value, element_type& elements){
		elements += value;
		value = elements;
	}

	static element_type fusion_args(const std::vector<element_type>& values){
		element_type res;
		if(!values.empty()){
			size_t nbvar = len(values[0]);
			for(size_t i = 0; i < nbvar; ++i){
				element_type resi;
				for(std::vector<element_type>::const_iterator it = values.begin(); it != values.end(); ++it)
					resi.append((*it)[i]);
				res.append(resi);
			}
		}
		return res;
	}
};

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
		lp.append(bp::object(pyiter));
		for (size_t i = 1; i < pattern->argSize(); ++i) lp.append(bp::object());
		params += lp;
	}
	else params.append(pyiter);

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

template<
template < typename, typename> class NextElement = StringNext, 
class Iterator = AxialTree::const_iterator, class PIterator = AxialTree::const_iterator,
class ArgsContainer = BPListManager >
struct StringMatcher : public ArgsContainer
{
	typedef typename ArgsContainer::element_type argtype;
	typedef NextElement<Iterator,PIterator> Next;

	static bool match(Iterator matching_start, Iterator  string_end,
					  PIterator pattern_begin, PIterator  pattern_end, 
					  Iterator& matching_end, Iterator& last_matched,
					  argtype& params)
	{

		Iterator it = matching_start;
		Iterator pit = it;
		argtype lp;
		for (PIterator it2 = pattern_begin; it2 != pattern_end; ++it2){
			pit = it;
			argtype lmp;
			if( it == string_end || !MatchingEngine::module_match(*it,*it2,lmp))return false;
			else { append_args(lp,lmp); it = Next::next(it,it2,string_end); }
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
class Iterator = AxialTree::const_iterator, class PRIterator = AxialTree::const_reverse_iterator,
class ArgsContainer = BPListManager >
struct StringReverseMatcher : public ArgsContainer
{
	typedef typename ArgsContainer::element_type argtype;
	typedef PreviousElement<Iterator,PRIterator> Previous;

	static bool match(Iterator matching_start, Iterator  string_begin, Iterator  string_end,
					  PRIterator pattern_rbegin, PRIterator  pattern_rend, 
					  Iterator& matching_end, argtype& params)
	{
		Iterator it = matching_start;
		argtype lp;
		for (PRIterator it2 = pattern_rbegin; it2 != pattern_rend; ){
			argtype lmp;
			if(!MatchingEngine::module_match(*it,*it2,lmp)) return false; 
			else { 
				++it2;
				if (it == string_begin){
					if (it2 != pattern_rend) return false;
				}
				else it = Previous::next(it,it2,string_begin,string_end);
				prepend_args(lp,lmp);
			}
		}
		params = lp;
		matching_end = it;
		return true; 
	}
};

/*---------------------------------------------------------------------------*/

template<
template < typename, typename > class FatherElement = GetFather, 
class Iterator = AxialTree::const_iterator, class RPIterator = AxialTree::const_reverse_iterator,
class ArgsContainer = BPListManager >
struct TreeLeftMatcher : public ArgsContainer
{
	typedef typename ArgsContainer::element_type argtype;
	typedef FatherElement<Iterator,RPIterator> Father;

	static bool match(Iterator matching_start, Iterator  string_begin, Iterator  string_end,
		RPIterator pattern_rbegin, RPIterator  pattern_rend, Iterator& matching_end,
		argtype& params)
	{
		Iterator it = matching_start;
		if(it == string_begin)return false;
		RPIterator it2 = pattern_rbegin;
		it = Father::next(it,it2,string_begin,string_end);
		argtype lparams;
		while(it2 != pattern_rend && it != string_end){
			argtype lp;
			if (! MatchingEngine::module_match(*it,*it2,lp) ) return false;
			prepend_args(lparams,lp);
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
			prepend_args(params,lparams);
			return true;
		}
		else return false;
	}
};
/*---------------------------------------------------------------------------*/

template<
template < typename, typename > class _NextElement = GetNext,
class Iterator = AxialTree::const_iterator, class PIterator = AxialTree::const_iterator,
class ArgsContainer = BPListManager >
struct TreeRightMatcher : public ArgsContainer
{
	typedef typename ArgsContainer::element_type argtype;
	typedef _NextElement<Iterator,PIterator> NextElement;
	typedef TreeRightMatcher<_NextElement,Iterator,PIterator,ArgsContainer> MType;

	static bool match(Iterator matching_start, Iterator  string_end,
					  PIterator pattern_begin, PIterator  pattern_end, 
					  Iterator last_matched, Iterator& matching_end, 
					  argtype& params)
	{
		Iterator it = matching_start;
		PIterator it2 = pattern_begin;
		argtype lparams;
		// printf("m '%s' - '%s'\n",it2->name().c_str(),it->name().c_str());
		it = NextElement::initial_next(it,it2,last_matched,string_end);				
		bool nextpattern = true;
		bool nextsrc = true;
		while(it != string_end && it2 != pattern_end){
			nextpattern = true;
			nextsrc = true;
			if(it2->isStar()){
				argtype lp;
				if(MatchingEngine::module_match(*it,*it2,lp)){ 
					append_args(lparams,lp);
				}
				else return false;
			}
			else if(it2->isGetIterator()){
				process_get_iterator(it2,it,string_end,lparams);
				nextsrc = false;
			}
			else if(it2->isRepExp()){
				std::vector<argtype> llp;
				AxialTree lpattern = bp::extract<AxialTree>(it2->getAt(0))();
				size_t miniter = 0;
				if (it2->argSize() > 1) miniter = bp::extract<size_t>(it2->getAt(1))();
				size_t maxiter = 1000;
				if (it2->argSize() == 3) maxiter = bp::extract<size_t>(it2->getAt(2))();
				else if (it2->argSize() == 2) maxiter = miniter;
				bool ok = true;
				size_t numiter = 0;
				while(ok && numiter < maxiter) {
					argtype lp;
					if((ok = MType::match(it,string_end,lpattern.begin(),lpattern.end(),last_matched,it,lp)))
					{  llp.push_back(lp); last_matched = it; ++it; ++numiter; }
			     }
				if (numiter < miniter) return false;
				--it;
				if(numiter == 0){
					size_t nbvar = lpattern.getVarNb();
					for(size_t i = 0; i < nbvar; ++i){
						lparams.append(argtype());
					}
				}
				else { 
					append_args(lparams,fusion_args(llp)); 
				}
			}
			else if(!it2->isBracket()){ // matching a pattern module
				if(!it->isBracket()) {
					argtype lp; // if not a bracket, try to match
					if(MatchingEngine::module_match(*it,*it2,lp)){
						append_args(lparams,lp); 
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
