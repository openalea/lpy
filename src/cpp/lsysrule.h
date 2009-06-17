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

#ifndef __PGL_LSYSRULE_H__
#define __PGL_LSYSRULE_H__

#include "axialtree.h"
#include "argcollector.h"

/*---------------------------------------------------------------------------*/

LPY_BEGIN_NAMESPACE

class LPY_API LsysRule {

public:

	// LsysRule();
	LsysRule(const LsysRule&);
	LsysRule(size_t = 0, size_t = 0, char prefix = 'p', int lineno = -1);
	~LsysRule();

	void set( const std::string& code );
	
	size_t getId() const { return __id; }
	void setId(size_t id) { __id = id; }

	size_t getGroupId() const { return __gid; }
	void setGroupId(size_t id) { __gid = id; }

	boost::python::object apply() const;
	boost::python::object apply( const ArgList& args ) const ;
//	boost::python::object apply( const boost::python::tuple& args ) const;

	bool compiled() const ;
	void compile();
	void importPyFunction();

	void clear();
	
	const AxialTree& predecessor() const
	{ return __predecessor; }
	
	const AxialTree& leftContext() const
	{ return __leftcontext; }
	
	const AxialTree& newLeftContext() const
	{ return __newleftcontext; }
	
	const AxialTree& rightContext() const
	{ return __rightcontext; }
	
	const AxialTree& newRightContext() const
	{ return __newrightcontext; }
	
	const std::string& definition() const
	{ return __definition; }
	
	const std::vector<std::string>& formalParameters() const
	{ return __formalparameters; }
	
	size_t nbParameters() const 
	{ return __formalparameters.size(); }
	
	size_t nbContexts() const;

	bool isContextFree() const
	{ return nbContexts() == 0; }

	bool hasQuery() const
	{ return __hasquery; }

	const boost::python::object& function() const
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
               eDirection direction = eForward) const ;

    inline bool reverse_match(const AxialTree& src,
			   AxialTree::const_iterator pos,
			   const AxialTree& dest,
			   AxialTree::const_iterator& endpos,
               ArgList& args) const 
    { return match(src,pos,dest,endpos,args,eBackward); }

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

	std::string functionName() const ;
	std::string callerFunctionName() const ;
	std::string name() const ;
	std::string uname() const ;
	
	std::string getCode() const;
	std::string getCoreCode() const;
	std::string getCallerCode() const;
	
	int redundantParameter() const;

	int lineno;
protected:

	void parseHeader( const std::string& name);
	void parseParameters();

	size_t __id;
	size_t __gid;
    char __prefix;
	AxialTree __predecessor;
	AxialTree __leftcontext;
	AxialTree __newleftcontext;
	AxialTree __rightcontext;
	AxialTree __newrightcontext;
	std::vector<std::string> __formalparameters;
	std::string __definition;
	boost::python::object __function;
	bool __hasquery;
private:
    void __precall_function( size_t nbargs = 0 ) const;
    void __precall_function( size_t nbargs,  const ArgList& obj ) const;
    boost::python::object __call_function( size_t nbargs,  const ArgList& obj ) const;
    boost::python::object __postcall_function( boost::python::object ) const;

};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
