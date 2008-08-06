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


/*---------------------------------------------------------------------------*/

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class ParamModule;
class ModuleClass;
typedef RCPtr<ModuleClass> ModuleClassPtr;
typedef std::vector<ModuleClassPtr> ModuleClassList;


class LPY_API ModuleClass : public TOOLS(RefCountObject) {
public:

	ModuleClass(const std::string& name);
	ModuleClass(const std::string& name, const std::string& alias);
	~ModuleClass();

	size_t getId() const { return id; }
	inline void activate(bool value = true) { active = value; }
	inline void desactivate() { active = false; }
	inline bool isActive() const { return active; }
	virtual void interpret(ParamModule& m, PGL::Turtle& t) ;
	virtual std::string getDocumentation() const { return ""; }
	virtual bool isPredefined() const { return false; }

	std::string name;
	std::vector<std::string> aliases;

	static ModuleClassList& getPredefinedClasses();
	static void clearPredefinedClasses();
	static void createPredefinedClasses();

	static ModuleClassPtr None;
	static ModuleClassPtr LeftBracket;
	static ModuleClassPtr RightBracket;
	static ModuleClassPtr ExactRightBracket;
	static ModuleClassPtr Cut;
	static ModuleClassPtr Star;
	static ModuleClassPtr QueryPosition;
	static ModuleClassPtr QueryHeading;
	static ModuleClassPtr QueryUp;
	static ModuleClassPtr QueryLeft;
	static ModuleClassPtr QueryRigth;
	static ModuleClassPtr F;
	static ModuleClassPtr f;
	static ModuleClassPtr X;
	static ModuleClassPtr StartGC;
	static ModuleClassPtr EndGC;
	static ModuleClassPtr StartPolygon;
	static ModuleClassPtr EndPolygon;
	static ModuleClassPtr SetPosition;
	static ModuleClassPtr SetHeading;
	static ModuleClassPtr Left;
	static ModuleClassPtr Right;
	static ModuleClassPtr Up;
	static ModuleClassPtr Down;
	static ModuleClassPtr RollL;
	static ModuleClassPtr RollR;
	static ModuleClassPtr TurnAround;
	static ModuleClassPtr RollToVert;
	static ModuleClassPtr Sphere;
	static ModuleClassPtr Circle;
	static ModuleClassPtr Label;
	static ModuleClassPtr IncWidth;
	static ModuleClassPtr DecWidth;
	static ModuleClassPtr SetWidth;
	static ModuleClassPtr IncColor;
	static ModuleClassPtr DecColor;
	static ModuleClassPtr SetColor;
	static ModuleClassPtr DivScale;
	static ModuleClassPtr MultScale;
	static ModuleClassPtr SetScale;
	static ModuleClassPtr Surface;
	static ModuleClassPtr CpfgSurface;
	static ModuleClassPtr PglShape;

protected:
	static ModuleClassList * PredefinedClasses;
private:
	size_t id;
	bool active;
	static size_t MAXID;
};

class LPY_API PredefinedModuleClass : public ModuleClass {
public:
	PredefinedModuleClass(const std::string& name, const std::string& documentation);
	PredefinedModuleClass(const std::string& name, const std::string& alias, const std::string& documentation);
	~PredefinedModuleClass();
	bool isPredefined() const { return true; }
	std::string documentation;
	std::string getDocumentation() const { return documentation; }
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

	ModuleClassPtr alias(const std::string& aliasname, const std::string& name);
	ModuleClassPtr getClass(const std::string&) ;
	ModuleClassPtr getClass(size_t id) const ;
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

	ModuleClassPtr find(std::string::const_iterator beg, std::string::const_iterator end);

protected:

	typedef  hash_map_string<ModuleClass *> ModuleClassMap;
    typedef STDEXT::hash_map<size_t,ModuleClass *> ModuleClassIdMap;
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


LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/
#endif

