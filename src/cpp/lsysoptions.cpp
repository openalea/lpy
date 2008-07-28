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

#include "lsysoptions.h"
#include "error.h"
#include "tracker.h"

LPY_USING_NAMESPACE

/*---------------------------------------------------------------------------*/

LsysOptionValue::LsysOptionValue(const std::string& _name, 
								 const std::string& _comment):
		name(_name), comment(_comment)
{
	IncTracker(LsysOptionValue)
}

LsysOptionValue::~LsysOptionValue() { DecTracker(LsysOptionValue) }

/*---------------------------------------------------------------------------*/

LsysOption::LsysOption(const std::string& _name, 
					   const std::string& _comment,
					   const std::string& _category):
		name(_name), comment(_comment), category(_category), global(false), current_value_id(0), default_value_id(0)
{
	IncTracker(LsysOption)
}

LsysOption::~LsysOption()
{
	DecTracker(LsysOption)
	for(OptionValueList::iterator it = __optionvalues.begin(); 
		it != __optionvalues.end(); ++it) delete *it;
}

bool LsysOption::activate(const std::string& optionvalue)
{
	for(OptionValueList::iterator it = __optionvalues.begin(); 
		it != __optionvalues.end(); ++it) {
			if ((*it)->name == optionvalue){
				current_value_id = distance(__optionvalues.begin(),it);
				(*it)->activate();
				return true;
			}
	}
	return false;
}

bool LsysOption::activateSelection() 
{
	__optionvalues[current_value_id]->activate();
	return true;
}

bool LsysOption::setSelection(const std::string& optionvalue)
{
	for(OptionValueList::iterator it = __optionvalues.begin(); 
		it != __optionvalues.end(); ++it) {
			if ((*it)->name == optionvalue){
				current_value_id = distance(__optionvalues.begin(),it);
				return true;
			}
	}
	return false;
}

bool LsysOption::setSelection(size_t value)
{
	if (value >=  __optionvalues.size())return false;
	current_value_id = value;
	valueChanged(current_value_id);
	return true;
}

void LsysOption::setCurrentValueId(size_t value)
{
	if (!setSelection(value))
		LsysError("Invalid Option Value");
}

void LsysOption::connectTo(UpdateSlot slot)
{
	__connectedSlots.push_back(slot);
}

void LsysOption::valueChanged(size_t value)
{
	for(UpdateSlotList::iterator it = __connectedSlots.begin(); it != __connectedSlots.end(); ++it)
		(**it)(value);
}

/*---------------------------------------------------------------------------*/

LsysOptions::LsysOptions()
{
	IncTracker(LsysOptions)
}

LsysOptions::~LsysOptions()
{
	DecTracker(LsysOptions)
	for(OptionList::iterator it = __options.begin();  it != __options.end(); ++it) delete *it;
}

LsysOption* LsysOptions::add(const std::string& name, const std::string& comment, const std::string& category)
{
	LsysOption * newoption = new LsysOption(name,comment,category);
	__options.push_back(newoption);
	return newoption;
}

LsysOption* LsysOptions::find(const std::string& name)
{
	for(OptionList::iterator it = __options.begin(); it != __options.end(); ++it) {
			if ((*it)->name == name){
				return *it;
			}
	}
	return NULL;
}

bool LsysOptions::activate(const std::string& optionname, const std::string& value)
{
	LsysOption* option = find(optionname);
	if(option == NULL) return false;
	return option->activate(value);
}

bool LsysOptions::activateSelection(const std::string& optionname)
{
	LsysOption* option = find(optionname);
	if(option == NULL) return false;
	return option->activateSelection();
}

bool LsysOptions::setSelection(const std::string& optionname, const std::string& value)
{
	LsysOption* option = find(optionname);
	if(option == NULL) return false;
	return option->setSelection(value);
}

bool LsysOptions::setSelection(const std::string& optionname, size_t valueid)
{
	LsysOption* option = find(optionname);
	if(option == NULL) return false;
	return option->setSelection(valueid);
}


/*---------------------------------------------------------------------------*/
