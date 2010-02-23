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

ModuleVTable::ModuleVTable(ModuleClassPtr owner, ModuleClassPtr base) : 
	__owner(owner.get()), __base(base.get()), scale(ModuleClass::DEFAULT_SCALE)  
{ 
	LsysContext::current()->__modulesvtables.push_back(ModuleVTablePtr(this));
	IncTracker(ModuleVTable)
	if(__base) updateInheritedParameters();
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

void ModuleVTable::setBase(ModuleClassPtr mclass) 
{ 
	__base = mclass; 
	updateInheritedParameters();
}

void ModuleVTable::updateInheritedParameters()
{
	if(__base){
		if(scale == ModuleClass::DEFAULT_SCALE){
			ModuleClass * base = __base;
			while (base != NULL && scale == ModuleClass::DEFAULT_SCALE){
				ModuleVTable * basevtable = base->__vtable;
				if( basevtable ) {
					scale = basevtable->scale;
					base = basevtable->__base;
				}
				else base = NULL;
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
