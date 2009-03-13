#ifndef __module_vtable_h__
#define __module_vtable_h__

#include "error.h"
#include <plantgl/tool/util_hashmap.h>
#include <plantgl/tool/rcobject.h>
#include <boost/python.hpp>

/*---------------------------------------------------------------------------*/

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/


class LPY_API BaseModuleProperty : public TOOLS(RefCountObject) {
public:
	BaseModuleProperty(const std::string& _name);
	virtual ~BaseModuleProperty();
	virtual bool isPythonProperty() const { return false; }
	std::string name;
};

typedef RCPtr<BaseModuleProperty> ModulePropertyPtr;

template<class Type>
class LPY_API ModuleProperty : public BaseModuleProperty {
public:
	typedef Type element_type;

	ModuleProperty(const std::string& name, element_type value) : 
		BaseModuleProperty(name), value(_value) {}

    element_type value;
  
};

class LPY_API ModulePyProperty : public ModuleProperty<boost::python::object> {
public:
	virtual bool isPythonProperty() const { return true; }
};

typedef RCPtr<ModulePyProperty> ModulePyPropertyPtr;

/*---------------------------------------------------------------------------*/

class ModuleClass;
typedef RCPtr<ModuleClass> ModuleClassPtr;


/// Module Virtual Table
class LPY_API ModuleVTable : public TOOLS(RefCountObject) {
public:
	typedef pgl_hash_map<std::string,ModulePropertyPtr> PropertyMap;
	friend class ModuleClass;
	ModuleVTable(ModuleClassPtr owner = ModuleClassPtr());
	~ModuleVTable();

	ModulePropertyPtr getProperty(const std::string& name) const;
	void setProperty(ModulePropertyPtr prop);
	bool removeProperty(const std::string& name);

	int scale;

	ModuleClassPtr getModuleClass() const { return __owner; }
	void setModuleClass(ModuleClassPtr mclass) { __owner = mclass; }

	void activate();
	void desactivate(); 

protected:

	PropertyMap __propertymap;
	ModuleClass * __owner;
};

typedef RCPtr<ModuleVTable> ModuleVTablePtr;
typedef std::vector<ModuleVTablePtr> ModuleVTableList;

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/

#endif