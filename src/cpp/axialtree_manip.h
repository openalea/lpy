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


/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
