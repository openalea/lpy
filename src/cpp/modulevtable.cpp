#include "modulevtable.h"
#include "lsyscontext.h"
#include "tracker.h"


/*---------------------------------------------------------------------------*/

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/


BaseModuleProperty::BaseModuleProperty(const std::string& _name): 
	  TOOLS(RefCountObject)(), name(name) { IncTracker(ModuleProperty) }

BaseModuleProperty::~BaseModuleProperty() { DecTracker(ModuleProperty) }

/*---------------------------------------------------------------------------*/

ModuleVTable::ModuleVTable(ModuleClassPtr owner) : 
	__owner(owner.get()), scale(ModuleClass::DEFAULT_SCALE)  
{ 
	LsysContext::current()->__modulesvtables.push_back(ModuleVTablePtr(this));
	IncTracker(ModuleVTable)
}

ModuleVTable::~ModuleVTable() { DecTracker(ModuleVTable) }

ModulePropertyPtr ModuleVTable::getProperty(const std::string& name) const
{
	PropertyMap::const_iterator it = __propertymap.find(name);
	if(it == __propertymap.end()) return it->second;
	else return ModulePropertyPtr();
}

void ModuleVTable::setProperty(ModulePropertyPtr prop)
{
	if(!prop || prop->name.empty())LsysError("Invalid property");
	__propertymap[prop->name] = prop;
}

bool ModuleVTable::removeProperty(const std::string& name)
{
	PropertyMap::iterator it = __propertymap.find(name);
	if(it == __propertymap.end()) return false;
	else { __propertymap.erase(it); return true; }
}

void ModuleVTable::activate()
{
	if(__owner)__owner->__vtable = ModuleVTablePtr(this);
}

void ModuleVTable::desactivate()
{
	if(__owner)__owner->__vtable = ModuleVTablePtr();
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
