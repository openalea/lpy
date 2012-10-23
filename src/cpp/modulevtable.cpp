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
__owner(owner.get()), __modulebases(is_null_ptr(base)?0:1,base.get()), scale(ModuleClass::DEFAULT_SCALE)  
{ 
	LsysContext::current()->__modulesvtables.push_back(ModuleVTablePtr(this));
	IncTracker(ModuleVTable)
	if(!__modulebases.empty()) updateInheritedParameters();
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
	__modulebases.clear();
	__modulebases.push_back(mclass.get()); 
	updateInheritedParameters();
}


void ModuleVTable::setBases(const ModuleClassList& mclass) 
{ 
	__modulebases.clear();
	for(ModuleClassList::const_iterator it = mclass.begin(); it != mclass.end(); ++it)
		__modulebases.push_back(it->get());			
	updateInheritedParameters();
}

bool ModuleVTable::issubclass(const ModuleClassPtr& mclass) const
{  return __modulebasescache.find(mclass->getId()) != __modulebasescache.end(); }

void ModuleVTable::updateInheritedParameters()
{
	if(!__modulebases.empty()){
		std::vector<ModuleClass *> bases = __modulebases;
		__modulebasescache.clear();
		while(!bases.empty()){
			ModuleClass * base = bases[0];
			if (base == __owner){
				__modulebasescache.clear();
				__modulebases.clear();
				LsysError("Cyclic inheritance");
			}
			bases.erase(bases.begin());
			__modulebasescache.insert(base->getId());
			if (base != NULL){
				ModuleVTable * basevtable = base->__vtable;
				if( basevtable ) {
					if(scale == ModuleClass::DEFAULT_SCALE) scale = basevtable->scale;
					bases.insert(bases.begin(),basevtable->__modulebases.begin(),basevtable->__modulebases.end());
				}
			}
		}
		std::vector<std::string> params =  __owner->getParameterNames();
		for(std::vector<ModuleClass *>::const_iterator it = __modulebases.begin(); it != __modulebases.end(); ++it){
			std::vector<std::string> iparams =  (*it)->getParameterNames();
			params.insert(params.end(),iparams.begin(),iparams.end());
		}
		__owner->setParameterNames(params);
	}
}

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
