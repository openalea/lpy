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


#include "moduleclass.h"
#include "lsyscontext.h"
#include "tracker.h"
#include <plantgl/tool/util_string.h>
#include <algorithm>
#include <iostream>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

ModuleClassTable& ModuleClassTable::get() { 
	if (!ModuleClassTable::__INSTANCE){
		ModuleClassTable::__INSTANCE = new ModuleClassTable();
	}
	return *__INSTANCE; 
}

ModuleClassTable * ModuleClassTable::__INSTANCE = NULL;

/*
Update 04/20: No garbage collector is needed as cleanLpy is registered with Py_AtExit and call cleanModuleClasses.
class ModuleClassTableGarbageCollector
{
public:
	ModuleClassTableGarbageCollector() {}
	~ModuleClassTableGarbageCollector() { 
		// std::cerr  << "module classes garbage collector" << std::endl;
		if (ModuleClassTable::__INSTANCE)
			ModuleClassTable::clearModuleClasses();
	}
protected:
	static ModuleClassTableGarbageCollector __INSTANCE;
};

ModuleClassTableGarbageCollector ModuleClassTableGarbageCollector::__INSTANCE;
*/

void ModuleClassTable::clearModuleClasses()
{
	ModuleClass::clearPredefinedClasses();

	if (ModuleClassTable::__INSTANCE) {
			delete ModuleClassTable::__INSTANCE; 
			ModuleClassTable::__INSTANCE = NULL; 
	}
}

/*---------------------------------------------------------------------------*/

size_t ModuleClass::MAXID(0);
int ModuleClass::DEFAULT_SCALE = INT_MAX;
size_t ModuleClass::NOPOS = std::string::npos;

/*---------------------------------------------------------------------------*/

ModuleClass::ModuleClass(const std::string& _name):
TOOLS(RefCountObject)(), name(_name), onlyInPattern(false), id(MAXID), active(true) {MAXID++; IncTracker(ModuleClass) }

ModuleClass::ModuleClass(const std::string& _name, const std::string& alias):
TOOLS(RefCountObject)(), name(_name), onlyInPattern(false), id(MAXID), active(true) {
	MAXID++; 
	aliases.push_back(alias);
	IncTracker(ModuleClass)
}

ModuleClass::~ModuleClass()
{ 
	// std::cerr << "Delete module class '" << name << "' with id " << id << " ... done." << std::endl;
	if(ModuleClassTable::__INSTANCE) 
		ModuleClassTable::__INSTANCE->remove(this);
	if (id == MAXID-1) --MAXID;
	DecTracker(ModuleClass)
}


void ModuleClass::interpret(ParamModule& m, PGL::Turtle& t) { 
}


void ModuleClass::activate(bool value) 
{	
	active = value; 
    if (!active) {
		if(__vtable)__vtable->desactivate(); 
    }
	else {
		if (!ModuleClassTable::get().isDeclared(this))
			ModuleClassTable::get().declare(this);
    }
}

void ModuleClass::create_vtable()
{
	__vtable = new ModuleVTable(this);
}

void ModuleClass::setProperty(ModulePropertyPtr prop)
{
	if(!__vtable)create_vtable();
	__vtable->setProperty(prop);
}

bool ModuleClass::removeProperty(const std::string& name)
{
	if(__vtable)return __vtable->removeProperty(name);
	else return false;
}

void ModuleClass::setBases(const ModuleClassList& bases)
{
	if(!__vtable)create_vtable();
	__vtable->setBases(bases);
}

ModuleClassList ModuleClass::getBases() const
{
	if(!__vtable) return ModuleClassList();
	else return __vtable->getBases();
}



void ModuleClass::setScale(int scale)
{
	if(!__vtable)create_vtable();
	__vtable->scale = scale;
}

/*---------------------------------------------------------------------------*/

void ModuleClass::setParameterNames(const std::vector<std::string>& names){
	__paramnames.clear();
	std::vector<std::string>::const_iterator itname = names.begin();
	for(size_t id = 0; itname != names.end(); ++itname,++id){
		// printf("'%s' %i : %s\n",name.c_str(),id,itname->c_str());
		__paramnames[*itname] = id;
	}
}

const ParameterNameDict * ModuleClass::sorter = NULL;
bool ModuleClass::sortNames(const std::string& a, const std::string& b) 
{  return sorter->find(a)->second < sorter->find(b)->second; }

std::vector<std::string> ModuleClass::getParameterNames() const
{
	std::vector<std::string> res;
	for(ParameterNameDict::const_iterator itname = __paramnames.begin(); itname != __paramnames.end(); ++itname)
		res.push_back(itname->first);
	sorter = &__paramnames;
	std::sort(res.begin(),res.end(),sortNames);
	sorter = NULL;
	return res;
}

size_t ModuleClass::getParameterPosition(const std::string& name) const{
	ParameterNameDict::const_iterator res = __paramnames.find(name);
	if(res != __paramnames.end()) return res->second;
	else return NOPOS;
}

/*---------------------------------------------------------------------------*/

ModuleClassTable::ModuleClassTable():
mandatory_declaration(false), maxnamelength(0)
{
	IncTracker(ModuleClassTable)
	registerPredefinedModule();
}

ModuleClassTable::~ModuleClassTable()
{
	clear();
	ModuleClassTable::__INSTANCE = NULL;
	DecTracker(ModuleClassTable)
}

void
ModuleClassTable::clear()
{
	modulenamemap.clear();
}

void 
ModuleClassTable::reset()
{
	clear();
	registerPredefinedModule();
}

/*---------------------------------------------------------------------------*/

void 
ModuleClassTable::registerPredefinedModule()
{
	for(ModuleClassList::const_iterator it = ModuleClass::getPredefinedClasses().begin();
		it != ModuleClass::getPredefinedClasses().end(); ++it) 
		declare(it->get());
}


/*---------------------------------------------------------------------------*/

ModuleClassPtr 
ModuleClassTable::declare(const std::string& name)
{
	ModuleClassMap::iterator itname;
	if ((itname = modulenamemap.find(name)) == modulenamemap.end())
	{
		ModuleClassPtr info = new ModuleClass(name);
		if(maxnamelength < name.size())maxnamelength = name.size();
		modulenamemap[name] = info.get();
		modulenamelist[info->getId()] = info.get();
		// std::cerr << "declare '" << name << "' first time with id " << info->getId() << std::endl;
		return info;
	}
	else {
		if (itname->second->isActive())
			LsysWarning("Redeclaration of module '"+name+"'.");
		else {
			// std::cerr << "redeclare '" << name << "' with id " << itname->second->getId() <<". reusing." << std::endl;
			itname->second->activate();
		}
		return itname->second;
	}
}

bool ModuleClassTable::declare(ModuleClass * moduleclass)
{
	if (modulenamelist.find(moduleclass->getId()) != modulenamelist.end()){
		LsysWarning("Redeclaration of predefined module '"+moduleclass->name+"'.");
		return false;
	}
	modulenamemap[moduleclass->name] = moduleclass;
	modulenamelist[moduleclass->getId()] = moduleclass;
	if(maxnamelength < moduleclass->name.size())maxnamelength = moduleclass->name.size();
	for(std::vector<std::string>::iterator it = moduleclass->aliases.begin(); it != moduleclass->aliases.end(); ++it){
		modulenamemap[*it] = moduleclass;
		if(maxnamelength < it->size())maxnamelength = it->size();
	}
	return true;
}

bool ModuleClassTable::isDeclared(const ModuleClass * moduleclass) const
{
	return (modulenamelist.find(moduleclass->getId()) != modulenamelist.end());
}


ModuleClassPtr 
ModuleClassTable::alias(const std::string& aliasname, const std::string& name)
{
	ModuleClassMap::iterator itname;
	ModuleClassMap::iterator italias;
	if((itname = modulenamemap.find(name)) == modulenamemap.end() || !itname->second->isActive()){
		if(itname == modulenamemap.end())LsysError("Undefined module '"+name+"' for alias.");
		else LsysError("Inactive module '"+name+"' for alias.");
	}
	if ((italias = modulenamemap.find(aliasname)) == modulenamemap.end())
	{
		ModuleClass * info = itname->second;
		if(maxnamelength < aliasname.size())maxnamelength = aliasname.size();
		info->aliases.push_back(aliasname);
		modulenamemap[aliasname] = info;
		return info;
	}
	else {
		if (*italias == *itname){
			LsysWarning("Redeclaration of alias '"+aliasname+"'.");
			return itname->second;
		}
		else {
			if (itname->second->isActive()){
			    LsysError("Redeclaration of alias '"+aliasname+"' as '"+name+"' (previously '"+italias->second->name+"').");
				return italias->second;
			}
			else {
				remove(itname->second);
				ModuleClass * info = itname->second;
				info->aliases.push_back(aliasname);
				modulenamemap[aliasname] = info;
				return info;
			}
		}
	}
}

void 
ModuleClassTable::alias(const std::string& aliasname, ModuleClassPtr module)
{
	ModuleClassMap::iterator italias;
	if(!module->isActive()){
		LsysError("Inactive module '"+module->name+"' for alias.");
	}
	if ((italias = modulenamemap.find(aliasname)) == modulenamemap.end())
	{
		if(maxnamelength < aliasname.size())maxnamelength = aliasname.size();
		module->aliases.push_back(aliasname);
		modulenamemap[aliasname] = module;
	}
	else {
		LsysError("Redeclaration of alias '"+aliasname+"' as '"+module->name+"' (previously '"+italias->second->name+"').");
	}
}



ModuleClassPtr
ModuleClassTable::getClass(const std::string& name)
{
	ModuleClassMap::const_iterator itname;
	if((itname = modulenamemap.find(name)) == modulenamemap.end()){
		if (mandatory_declaration) LsysError("Undefined module '"+name+"'.");
		else return declare(name);
	}
	else if(!itname->second->isActive()){
		if (mandatory_declaration) LsysError("Undefined module '"+name+"'.");
		else { itname->second->activate(); return itname->second; }
	}
	return itname->second;
}

ModuleClassPtr
ModuleClassTable::find(const std::string& name) const
{
	ModuleClassMap::const_iterator itname;
	if((itname = modulenamemap.find(name)) == modulenamemap.end() || (!itname->second->isActive()))
		return ModuleClassPtr(0);
	return itname->second;
}

ModuleClassPtr
ModuleClassTable::find(size_t id) const
{
	ModuleClassIdMap::const_iterator it;
	if((it = modulenamelist.find(id)) == modulenamelist.end() || !it->second->isActive())
		LsysError("Undefined module with id "+TOOLS(number)(id)+".");
	return it->second;
}

ModuleClassList ModuleClassTable::getClasses() const {
	ModuleClassList res;
	for(ModuleClassIdMap::const_iterator itid = modulenamelist.begin();
		itid != modulenamelist.end(); ++itid)
		if(itid->second->isActive())
			res.push_back(itid->second);
	return res;
}

std::vector<std::string> ModuleClassTable::getNames() const {
	std::vector<std::string> res;
	for(ModuleClassMap::const_iterator it = modulenamemap.begin();
		it != modulenamemap.end(); ++it)
		if(it->second->isActive())
			res.push_back(it->first);
	return res;
}

bool 
ModuleClassTable::remove(const std::string& name)
{
	ModuleClassMap::iterator it;
	std::vector<std::string>::iterator aliasiter;
	if((it = modulenamemap.find(name)) != modulenamemap.end() && it->second->isActive()){
		if (it->second->name == name)
		{   
			if (it->second->aliases.size() > 0) {
				// if has alias, rename class using first alias name
				it->second->name = it->second->aliases[0];
				it->second->aliases.erase(it->second->aliases.begin());
			}
			else {
				// no more name reference this modclass, so remove it from idmap.
				ModuleClassIdMap::iterator itid = modulenamelist.find(it->second->getId());
				if (itid != modulenamelist.end())modulenamelist.erase(itid);
			}
		}
		else if((aliasiter = std::find(it->second->aliases.begin(),it->second->aliases.end(),name)) != it->second->aliases.end())
		{   // remove name from aliases
			it->second->aliases.erase(aliasiter); 
		}
		// remove name
		modulenamemap.erase(it);
		return true;
	}
	return false;
}


bool ModuleClassTable::remove(const ModuleClass * moduleclass)
{
	bool has_removed = false;
	for(ModuleClassMap::iterator itname = modulenamemap.begin(); itname != modulenamemap.end(); ){
		if (itname->second == moduleclass){
			ModuleClassMap::iterator itname2 = itname;
			itname2++;
			modulenamemap.erase(itname);
			has_removed = true;
			itname = itname2;
		}
		else ++itname;
	}
	ModuleClassIdMap::iterator itid = modulenamelist.find(moduleclass->getId());
	if (itid != modulenamelist.end())modulenamelist.erase(itid);
	return has_removed;
}

/*---------------------------------------------------------------------------*/

ModuleClassPtr 
ModuleClassTable::parse(std::string::const_iterator beg, 
					   std::string::const_iterator end,
					   size_t& nsize)
{
	size_t wordlength = std::min<size_t>(maxnamelength+1,std::distance(beg,end));
	ModuleClassMap::iterator itname;
	for (size_t w = wordlength; w > 0; --w){
		itname = modulenamemap.find(std::string(beg,beg+w));
		if(itname != modulenamemap.end() && itname->second->isActive()){
			nsize = w;
			return itname->second;
		}
	}
	if (mandatory_declaration) return ModuleClassPtr();
	else { 
		ModuleClassPtr modclass = declare(*beg);
		LsysContext::currentContext()->declare(modclass);
		nsize = 1;
		return modclass;
	}
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE
