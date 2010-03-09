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
__owner(owner.get()), __bases(is_null_ptr(base)?0:1,base.get()), scale(ModuleClass::DEFAULT_SCALE)  
{ 
	LsysContext::current()->__modulesvtables.push_back(ModuleVTablePtr(this));
	IncTracker(ModuleVTable)
	if(!__bases.empty()) updateInheritedParameters();
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
	__bases.push_back(mclass.get()); 
	updateInheritedParameters();
}


void ModuleVTable::setBases(const ModuleClassList& mclass) 
{ 
	for(ModuleClassList::const_iterator it = mclass.begin(); it != mclass.end(); ++it)
		__bases.push_back(it->get());			
	updateInheritedParameters();
}

bool ModuleVTable::issubclass(const ModuleClassPtr& mclass) const
{  return __basescache.find(mclass->getId()) != __basescache.end(); }

void ModuleVTable::updateInheritedParameters()
{
	if(!__bases.empty()){
		std::vector<ModuleClass *> bases = __bases;
		__basescache.clear();
		while(!bases.empty()){
			ModuleClass * base = bases[0];
			if (base == __owner){
				__basescache.clear();
				__bases.clear();
				LsysError("Cyclic inheritance");
			}
			bases.erase(bases.begin());
			__basescache.insert(base->getId());
			if (base != NULL){
				ModuleVTable * basevtable = base->__vtable;
				if( basevtable ) {
					if(scale == ModuleClass::DEFAULT_SCALE) scale = basevtable->scale;
					bases.insert(bases.begin(),basevtable->__bases.begin(),basevtable->__bases.end());
				}
			}
		}
		std::vector<std::string> params =  __owner->getParameterNames();
		for(std::vector<ModuleClass *>::const_iterator it = __bases.begin(); it != __bases.end(); ++it){
			std::vector<std::string> iparams =  (*it)->getParameterNames();
			params.insert(params.end(),iparams.begin(),iparams.end());
		}
		__owner->setParameterNames(params);
	}
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
