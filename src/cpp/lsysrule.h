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

#ifndef __lpy_lsysrule_h__
#define __lpy_lsysrule_h__

#include "axialtree.h"
#include "patternstring.h"
#include "argcollector.h"
#include "paramproduction.h"
#include "lstringmatcher.h"
#include "consider.h"

/*---------------------------------------------------------------------------*/

class PglMutex;

LPY_BEGIN_NAMESPACE


class LPY_API LsysRule {

public:

	// LsysRule();
	LsysRule(const LsysRule&);
	LsysRule(size_t = 0, size_t = 0, char prefix = 'p', int lineno = -1);
	~LsysRule();

	void set( const std::string& code );

	void consider(const ConsiderFilterPtr consider);
	void consider(const std::string& modules);
	void ignore(const std::string& modules);
	ConsiderFilterPtr getConsiderFilter() const { return __consider; }

	inline size_t getId() const { return __id; }
	inline void setId(size_t id) { __id = id; }

	inline size_t getGroupId() const { return __gid; }
	inline void setGroupId(size_t id) { __gid = id; }

	AxialTree apply(bool * isApplied = NULL) const;
	AxialTree apply( const ArgList& args, bool * isApplied = NULL ) const ;
//	boost::python::object apply( const boost::python::tuple& args ) const;

	inline bool isCompiled() const {  return __function != boost::python::object(); }
	void compile();
	void recompile();
	void importPyFunction();

	void clear();
	
	inline const PatternString& predecessor() const
	{ return __predecessor; }
	
	inline const PatternString& leftContext() const
	{ return __leftcontext; }
	
	inline const PatternString& newLeftContext() const
	{ return __newleftcontext; }
	
	inline const PatternString& rightContext() const
	{ return __rightcontext; }
	
	inline const PatternString& newRightContext() const
	{ return __newrightcontext; }
	
	inline const std::string& definition() const
	{ return __definition; }
	
	inline const std::vector<std::string>& formalParameters() const
	{ return __formalparameters; }
	
	inline size_t nbParameters() const 
	{ return __nbParams; }
	
	size_t nbContexts() const;

	inline bool isContextFree() const
	{ return nbContexts() == 0; }

	inline bool hasQuery() const
	{ return __hasquery; }

	inline const boost::python::object& function() const
	{ return __function; }

    inline bool forwardCompatible() const 
        { return __newrightcontext.empty(); }

    inline bool backwardCompatible() const 
        { return __newleftcontext.empty(); }

    inline bool isCompatible(eDirection direction) const 
        { return (direction == eForward? forwardCompatible() : backwardCompatible()); }

	bool match(const AxialTree& src,
			   AxialTree::const_iterator pos,
			   const AxialTree& dest,
			   AxialTree::const_iterator& endpos,
			   ArgList& args,
               AxialTree::IteratorMap* itermap = NULL,
               eDirection direction = eForward) const ;

    inline bool reverse_match(const AxialTree& src,
			   AxialTree::const_iterator pos,
			   const AxialTree& dest,
			   AxialTree::const_iterator& endpos,
               ArgList& args,
               AxialTree::IteratorMap* itermap = NULL) const 
    { return match(src,pos,dest,endpos,args, itermap, eBackward); }

	bool applyTo( AxialTree& dest, 
				  const ArgList& args, 
				  size_t * length = NULL,
				  eDirection direction = eForward) const;

	inline bool reverseApplyTo( AxialTree& dest, 
				  const ArgList& args, 
				  size_t * length = NULL,
				  eDirection direction = eForward) const
	{ return applyTo(dest,args,length,eBackward); }

	AxialTree process( const AxialTree& src ) const;

	std::string str() const ;
    inline const char * c_str() const { return str().c_str(); }

	std::string functionName() const ;
	std::string callerFunctionName() const ;
	std::string name() const ;
	std::string uname() const ;
	
	std::string getCode() ;
	std::string getCoreCode() ;
	std::string getCallerCode() const;

	void setStatic();
	void keepOnlyRelevantVariables();

	int redundantParameter() const;

	int lineno;
	uint32_t getCodeLength() const { return __codelength; }

	inline bool isStatic() const { return __isStatic; }
	inline AxialTree getStaticProduction() const { return __staticResult; }

protected:

	void parseHeader( const std::string& name);
	void parseParameters();
	void initStaticProduction();

	size_t __id;
	size_t __gid;
    char __prefix;
	PatternString __predecessor;
	PatternString __leftcontext;
	PatternString __newleftcontext;
	PatternString __rightcontext;
	PatternString __newrightcontext;
	std::vector<std::string> __formalparameters;
	size_t __nbParams;
	std::string __definition;
	boost::python::object __function;
	bool __hasquery;
	bool __isStatic;
	AxialTree __staticResult;
	uint32_t __codelength;
	ConsiderFilterPtr __consider;
	LstringMatcherPtr __lstringmatcher;

private:
    void __precall_function( size_t nbargs = 0 ) const;
    void __precall_function( size_t nbargs,  const ArgList& obj ) const;
    boost::python::object __call_function( size_t nbargs,  const ArgList& obj ) const;
    AxialTree __postcall_function( boost::python::object, bool * isApplied = NULL ) const;

    PglMutex * mutex;

};

/*---------------------------------------------------------------------------*/

typedef std::vector<LsysRule> RuleSet;
typedef std::vector<const LsysRule *> RulePtrSet;

/*---------------------------------------------------------------------------*/

class RulePtrMap {
public:
	typedef std::vector<RulePtrSet> RulePtrSetMap;

	RulePtrMap();
	RulePtrMap(const RulePtrSet& rules, eDirection direction = eForward);

	inline const RulePtrSet& operator[](size_t id) const 
	{ return (id < __maxsmb?__map[id]:__defaultset); }
	inline bool empty() const {  return __nbrules == 0; }
	inline size_t size() const { return __nbrules; }

protected:
	RulePtrSetMap __map;
	RulePtrSet __defaultset;
	size_t __nbrules;
	size_t __maxsmb;

};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
