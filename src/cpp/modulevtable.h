#ifndef __module_vtable_h__
#define __module_vtable_h__

#include "error.h"
#include <plantgl/tool/util_hashmap.h>
#include <plantgl/tool/rcobject.h>
#include <boost/python.hpp>
#include <vector>

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

	ModuleProperty(const std::string& name, element_type _value) : 
		BaseModuleProperty(name), value(_value) {}

    element_type value;
  
};

class LPY_API ModulePyProperty : public ModuleProperty<boost::python::object> {
public:
	ModulePyProperty(const std::string& name, boost::python::object _value) : 
		ModuleProperty< boost::python::object > ( name, _value) {}

	virtual bool isPythonProperty() const { return true; }
};

typedef RCPtr<ModulePyProperty> ModulePyPropertyPtr;

/*---------------------------------------------------------------------------*/

class ModuleClass;
typedef RCPtr<ModuleClass> ModuleClassPtr;


/// Module Virtual Table
class LPY_API ModuleVTable : public TOOLS(RefCountObject) {
public:
	typedef pgl_hash_map_string<ModulePropertyPtr> PropertyMap;
	friend class ModuleClass;
	ModuleVTable(ModuleClassPtr owner = ModuleClassPtr(), ModuleClassPtr base = ModuleClassPtr());
	~ModuleVTable();

	ModulePropertyPtr getProperty(const std::string& name) const;
	void setProperty(ModulePropertyPtr prop);
	bool removeProperty(const std::string& name);

	int scale;

	inline ModuleClassPtr getModuleClass() const { return __owner; }
	inline void setModuleClass(ModuleClassPtr mclass) { __owner = mclass; }

	inline ModuleClassPtr getBase() const { return __base; }
	inline bool hasBaseClass() const { return __base == NULL; }
	void setBase(ModuleClassPtr mclass) ;

	void activate();
	void desactivate(); 

protected:
	void updateInheritedParameters() ;

	PropertyMap __propertymap;
	ModuleClass * __owner;
	ModuleClass * __base;

};

typedef RCPtr<ModuleVTable> ModuleVTablePtr;
typedef std::vector<ModuleVTablePtr> ModuleVTableList;

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/

#endif
