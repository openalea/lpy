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

#ifndef __LSYS_MODULECLASS_H__
#define __LSYS_MODULECLASS_H__

#include "error.h"
#include <vector>
#include <plantgl/tool/util_hashmap.h>
#include <plantgl/tool/rcobject.h>
#include <plantgl/algo/modelling/turtle.h>
#include "modulevtable.h"

/*---------------------------------------------------------------------------*/

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class ParamModule;
class ModuleClass;
typedef RCPtr<ModuleClass> ModuleClassPtr;
typedef std::vector<ModuleClassPtr> ModuleClassList;

#define PREDEFINED_MODULE_APPLY(MACRO) \
	MACRO(None) \
	MACRO(LeftBracket) \
	MACRO(RightBracket) \
	MACRO(ExactRightBracket) \
	MACRO(Cut) \
	MACRO(Star) \
	MACRO(RepExp) \
	MACRO(Or) \
	MACRO(QueryPosition) \
	MACRO(QueryHeading) \
	MACRO(QueryUp) \
	MACRO(QueryLeft) \
	MACRO(QueryRigth) \
	MACRO(QueryFrame) \
	MACRO(F) \
	MACRO(f) \
	MACRO(X) \
	MACRO(StartGC) \
	MACRO(EndGC) \
	MACRO(StartPolygon) \
	MACRO(EndPolygon) \
	MACRO(SetPosition) \
	MACRO(SetHeading) \
	MACRO(Left) \
	MACRO(Right) \
	MACRO(Up) \
	MACRO(Down) \
	MACRO(RollL) \
	MACRO(RollR) \
	MACRO(TurnAround) \
	MACRO(RollToVert) \
	MACRO(Sphere) \
	MACRO(Circle) \
	MACRO(Label) \
	MACRO(Frame) \
	MACRO(IncWidth) \
	MACRO(DecWidth) \
	MACRO(SetWidth) \
	MACRO(IncColor) \
	MACRO(DecColor) \
	MACRO(SetColor) \
	MACRO(DivScale) \
	MACRO(MultScale) \
	MACRO(SetScale) \
	MACRO(Surface) \
	MACRO(CpfgSurface) \
	MACRO(PglShape) \
	MACRO(Elasticity) \
	MACRO(Tropism) \
	MACRO(SetContour) \
	MACRO(SectionResolution) \
	MACRO(GetIterator) \
	MACRO(GetModule) \
	MACRO(New) \

#define DECLARE_PM(MName) static ModuleClassPtr MName;

class LPY_API ModuleClass : public TOOLS(RefCountObject) {
public:
    static int DEFAULT_SCALE;

	friend class ModuleVTable;
	friend class LsysContext;

	ModuleClass(const std::string& name);
	ModuleClass(const std::string& name, const std::string& alias);
	~ModuleClass();

	size_t getId() const { return id; }

	void activate(bool value = true) ;
	inline void desactivate() { activate(false); }

	inline bool isActive() const { return active; }
	virtual void interpret(ParamModule& m, PGL::Turtle& t) ;
	virtual std::string getDocumentation() const { return ""; }
	virtual bool isPredefined() const { return false; }

	std::string name;
	std::vector<std::string> aliases;

	static ModuleClassList& getPredefinedClasses();
	static void clearPredefinedClasses();
	static void createPredefinedClasses();

    PREDEFINED_MODULE_APPLY(DECLARE_PM)

	inline int getScale() const 
	{ if (__vtable) return __vtable->scale; 
	  else return DEFAULT_SCALE; }

	void setScale(int scale);

	inline ModulePropertyPtr getProperty(const std::string& name) const 
	{ if (__vtable) return __vtable->getProperty(name); else return ModulePropertyPtr(); }

	void setProperty(ModulePropertyPtr prop);
	bool removeProperty(const std::string& name);
	bool isOnlyInPattern() const { return onlyInPattern; }

	static size_t NOPOS;

	void setParameterNames(const std::vector<std::string>& names);
	std::vector<std::string> getParameterNames() const;
	size_t getParameterPosition(const std::string&) const;
	inline bool hasParameter(const std::string& name) const 
	{ return getParameterPosition(name) != NOPOS; }

	static size_t getMaxId() { return MAXID; }

protected:
	static ModuleClassList * PredefinedClasses;
	bool onlyInPattern;
private:
	size_t id;
	bool active;

	static size_t MAXID;

	ModuleVTablePtr __vtable;
	void create_vtable();

	typedef pgl_hash_map_string<size_t> InternalParameterNameList;
	InternalParameterNameList __paramnames;
	static const InternalParameterNameList * sorter;
	static bool sortNames(const std::string&,const std::string&);

};

class LPY_API PredefinedModuleClass : public ModuleClass {
    static const char * CATEGORY_NAME[];
public:
	enum eCategory {
		eNone = 0,
		eStructure,
		eRotation,
		ePosition,
		eScale,
		ePrimitive,
		eWidth,
		eColor,
	    eTropism,
		eRequest,
		eStringManipulation,
		ePatternMatching,
		eUserDefined,
		eLastCategory = eUserDefined
	} ;
	static std::string getCategoryName(eCategory);

	PredefinedModuleClass(const std::string& name, 
						  const std::string& documentation,
						  eCategory category = eNone);

	PredefinedModuleClass(const std::string& name, 
						  const std::string& alias, 
						  const std::string& documentation,
						  eCategory category = eNone);

	~PredefinedModuleClass();
	bool isPredefined() const { return true; }
	std::string getDocumentation() const { return documentation; }
	eCategory getCategory() const { return category; }
	std::string documentation;
	eCategory category;

};


class LPY_API ModuleClassTable {
public:
	friend class ModuleClass;

	// Singleton
	static ModuleClassTable& get();
	static void clearModuleClasses();

	ModuleClassPtr declare(const std::string& name);
	ModuleClassPtr declare(const char name) { std::string n(1,name); return declare(n); }
	bool declare(ModuleClass * moduleclass);
	bool isDeclared(const ModuleClass * moduleclass) const;

	ModuleClassPtr alias(const std::string& aliasname, const std::string& name);
	void alias(const std::string& aliasname, ModuleClassPtr module);
	/// get a class. If it does not exist, create it
	ModuleClassPtr getClass(const std::string&) ;
	/// find an existing class
	ModuleClassPtr findClass(size_t id) const ;
	/// find an existing class
	ModuleClassPtr findClass(const std::string&) const ;

	bool remove(const std::string& name);
	bool remove(const ModuleClass * moduleclass);

	void reset();
	size_t size() const { return modulenamelist.size(); }
	bool empty() const { return modulenamelist.empty(); }
	ModuleClassList getClasses() const ;
	std::vector<std::string> getNames() const ;

	static void setMandatoryDeclaration(bool value) { get().mandatory_declaration = value; }
	bool mandatory_declaration;

	ModuleClassPtr find(std::string::const_iterator beg, std::string::const_iterator end,
					    size_t& nsize);

protected:

	typedef  pgl_hash_map_string<ModuleClass *> ModuleClassMap;
    typedef pgl_hash_map<size_t,ModuleClass *> ModuleClassIdMap;
	ModuleClassMap  modulenamemap;
	ModuleClassIdMap modulenamelist;
	size_t maxnamelength;

	void clear();
	void registerPredefinedModule();

private:
	friend class ModuleClassTableGarbageCollector; 

	static ModuleClassTable * __INSTANCE;
	ModuleClassTable();
    ~ModuleClassTable();
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
#endif

