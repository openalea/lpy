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

#ifndef __axialtree_manip_h__
#define __axialtree_manip_h__

#include "axialtree.h"

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

template<class Iterator>
Iterator endBracket(Iterator pos, Iterator string_end, bool startingBeforePos = false)
{ 
  if(pos == string_end)return pos;
  if(!startingBeforePos && pos->isLeftBracket())++pos;
  int bracket= 0;
  while(pos != string_end && (bracket>0 || !pos->isRightBracket())){
	if(pos->isLeftBracket()) bracket++;
	else if(pos->isRightBracket()) bracket--;
	++pos;
  }
  return pos; 
}

template<class Iterator>
Iterator beginBracket(Iterator pos, Iterator string_begin, Iterator string_end, bool startingAfterPos = false)
{ 
  if(pos != string_end && pos->isLeftBracket())return pos;
  if(pos == string_begin) return string_end;
  if(pos == string_end)--pos;
  else if(!startingAfterPos && pos->isRightBracket())--pos;
  int bracket= 0;
  while(pos != string_begin && (bracket>0 || !pos->isLeftBracket())){
	if(pos->isRightBracket()) bracket++;
	else if(pos->isLeftBracket()) bracket--;
	--pos;
  }
  if(pos == string_begin) {
	if(bracket == 0 && pos->isLeftBracket())return pos;
	else return string_end;
  }
  return pos; 
}


template<class Iterator>
Iterator father(Iterator pos, Iterator string_begin, Iterator string_end)
{
  if( pos == string_begin ) return string_end;
  --pos;
  while((pos != string_end) && (pos != string_begin) && (pos->isBracket() || pos->isIgnored())){
	while( (pos != string_begin) && (pos->isLeftBracket() || pos->isIgnored()))--pos;
	while((pos != string_end) && (pos != string_begin) && pos->isRightBracket()){
	  pos = beginBracket(pos,string_begin,string_end);
	  if( pos != string_end ) --pos;
	}
  }
  if( pos == string_end ) return pos;
  else if( (pos == string_begin) && (pos->isLeftBracket() || pos->isIgnored())) return string_end;
  else return pos;
}

template<class Iterator>
bool wellBracketed(Iterator string_begin, Iterator string_end) 
{ 
  int bracket= 0;
  AxialTree::const_iterator pos = string_begin;
  while((pos != string_end) && bracket>= 0){
	if(pos->isLeftBracket()) ++bracket;
	else if(pos->isRightBracket()) --bracket;
	++pos;
  }
  return (bracket == 0); 
}

template<class Iterator>
std::vector<Iterator>  sons(Iterator pos, Iterator string_end)
{ 
  std::vector<Iterator> result; 
  if( (pos == string_end) || pos->isRightBracket()) return result;
  ++pos;
  while((pos != string_end) && (pos->isBracket() || pos->isIgnored())){
	while((pos != string_end) && !pos->isBracket() && pos->isIgnored())++pos;
	while((pos != string_end) && pos->isLeftBracket()){
	  std::vector<Iterator> res = sons(pos,string_end);
	  if(!res.empty())
		result.insert(result.end(),res.begin(),res.end());
	  pos = endBracket(pos,string_end);
	  if( pos == string_end ) return result;
	  ++pos;
	}
  }
  if( (pos != string_end) && !pos->isRightBracket()){
	result.push_back(pos);
  }
  return result;
}

template<class Iterator>
Iterator directSon(Iterator pos, Iterator string_end) 
{
  if( (pos == string_end) || pos->isRightBracket()) return string_end;
  ++pos;
  while((pos != string_end) && (pos->isLeftBracket() || pos->isIgnored())){
	while((pos != string_end) && !pos->isLeftBracket() && pos->isIgnored())++pos;
	while((pos != string_end) && pos->isLeftBracket()){
	  pos = endBracket(pos,string_end);
	  if( pos == string_end ) return string_end;
	  ++pos;
	}
  }
  if( (pos == string_end) || pos->isRightBracket()) return string_end;
  else return pos;
}


template<class Iterator>
std::vector<Iterator> lateralSons(Iterator pos, Iterator string_end) {
  std::vector<Iterator> result; 
  if( (pos == string_end) || pos->isRightBracket()) return result;
  ++pos;
  while((pos != string_end) && (pos->isBracket() || pos->isIgnored())){
	while((pos != string_end) && !pos->isBracket() && pos->isIgnored())++pos;
	while((pos != string_end) && pos->isLeftBracket()){
	  std::vector<Iterator> res = sons(pos,string_end);
	  result.insert(result.end(),res.begin(),res.end());
	  pos = endBracket(pos,string_end);
	  if( pos == string_end ) return result;
	  ++pos;
	}
  }
  return result;
}

template<class Iterator>
std::vector<Iterator> roots(Iterator string_begin, Iterator string_end) 
{ 
  std::vector<Iterator> res;
  if (string_begin == string_end) return res;
  AxialTree::const_iterator i = string_begin;
  if(i->isRightBracket())return res;
  else if(i->isLeftBracket()){
	while((i != string_end) && (i->isBracket() || i->isIgnored())){
	  while((i != string_end) && i->isLeftBracket()){
		std::vector<Iterator> res2 = sons(i,string_end);
		if(!res2.empty())
		  res.insert(res.end(),res2.begin(),res2.end());
		i = endBracket(i,string_end);
		if( i == string_end ) return res;
		++i;
	  }
	  while((i != string_end) && !i->isBracket() && i->isIgnored())++i;
	}
	if( (i != string_end) && !i->isRightBracket()){
	  res.push_back(i);
	}
	// std::vector<const_iterator> res2 = sons(i);
	// if(!res2.empty())res.insert(res.end(),res2.begin(),res2.end());
  }
  else if(i->isIgnored()){
	std::vector<Iterator> res2 = sons(i,string_end);
	if(!res2.empty())res.insert(res.end(),res2.begin(),res2.end());
  }
  else res.push_back(i);
  return res;
}

template<class Iterator>
Iterator complex(Iterator pos, int scale, Iterator string_begin, Iterator string_end)
{
  if( pos == string_begin ) return string_end;
  if( !is_lower_scale(pos->scale(),scale)) return string_end;
  pos = father(pos,string_begin, string_end);
  while(pos != string_end && is_lower_scale(pos->scale(),scale)){
	  pos = father(pos,string_begin, string_end);
  }
  if (pos == string_end) return string_end;
  if (is_eq_scale(pos->scale(),scale))  return pos;
  else return string_end;
}

template<class Iterator>
Iterator predecessor_at_scale(Iterator pos, int scale, Iterator string_begin, Iterator string_end)
{
  if( pos == string_begin ) return string_end;
  pos = father(pos,string_begin, string_end);
  while(pos != string_end && is_lower_scale(pos->scale(),scale)){
	  pos = father(pos,string_begin, string_end);
  }
  if (pos == string_end) return string_end;
  if (is_eq_scale(pos->scale(),scale))  return pos;
  else return string_end;
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
