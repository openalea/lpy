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

#ifndef __PGL_LSYSTEM_H__
#define __PGL_LSYSTEM_H__

#include "lsysrule.h"
#include "lsyscontext.h"
#include "stringmatching.h"
#include <QtCore/QMutex>

LPY_BEGIN_NAMESPACE

#define MULTI_THREADED_LSYSTEM


/*---------------------------------------------------------------------------*/

class LPY_API Lsystem {
public:
  /** The different type of rules */
  enum eRuleType {
      eProduction,
      eDecomposition,
      eInterpretation
  };

  /** Constructor and Destructor */
  Lsystem();
  Lsystem(const std::string& filename);
  Lsystem(const std::string& filename, const boost::python::dict& parameters);
  ~Lsystem();

  /** clear */
  void clear();

  /** rules nb query */
  bool empty() const;
  size_t nbProductionRules( size_t group ) const;
  size_t nbDecompositionRules( size_t group ) const ;
  size_t nbInterpretationRules( size_t group ) const;
  size_t nbTotalRules( ) const;
  size_t nbGroups( ) const;

  /** print */
  std::string str() const ;
  inline const char * c_str() const { return str().c_str(); }
  std::string code() ;

  /** compile */
  bool isCompiled();
  void compile();

  /** Filename */
  void setFilename( const std::string& filename );
  std::string getFilename( ) const ;
  std::string getShortFilename( ) const ;

  /** build from file/string */
  void read( const std::string& filename, 
			 const boost::python::dict& parameters = boost::python::dict() );

  void set( const std::string& rules , 
		    std::string * pycode = NULL, 
			const boost::python::dict& parameters = boost::python::dict());

  /** set rules */
  void addRule( const LsysRule& rule, int type, size_t group );

  inline void addRule( const std::string& rule, int type, size_t group)
  { addRule(rule, type, group,  ConsiderFilterPtr()); }
  void addRule( const std::string& rule, int type, size_t group, const ConsiderFilterPtr filter);

  inline void addProductionRule( const std::string& rule, size_t group)
  { addProductionRule( rule, group, ConsiderFilterPtr() ); }
  void addProductionRule( const std::string& rule, size_t group, const ConsiderFilterPtr filter);

  inline void addDecompositionRule( const std::string& rule, size_t group )
  {  addDecompositionRule( rule, group, ConsiderFilterPtr() ); }
  void addDecompositionRule( const std::string& rule, size_t group, const ConsiderFilterPtr filter);

  inline void addInterpretationRule( const std::string& rule, size_t group )
  { addInterpretationRule( rule, group, ConsiderFilterPtr() ); }
  void addInterpretationRule( const std::string& rule, size_t group, const ConsiderFilterPtr filter);

  /** get rules */
  inline const LsysRule& productionRule( size_t i, size_t group) const
  { return __rules[group].production[i]; }
  inline const LsysRule& decompositionRule( size_t i, size_t group) const
  { return __rules[group].decomposition[i]; }
  inline const LsysRule& interpretationRule( size_t i, size_t group) const
  { return __rules[group].interpretation[i]; }

  /** Axiom */
  void setAxiom( const AxialTree& axiom );
  const AxialTree& getAxiom( ) const ;

  /** Plot and interpret */
  void plot( AxialTree& workstring, bool checkLastComputedScene =  false);

  inline void turtle_interpretation( AxialTree& workstring)
  { turtle_interpretation(workstring,__context.envturtle); }
  void turtle_interpretation( AxialTree& workstring, PGL::Turtle& t );
  PGL(ScenePtr) sceneInterpretation( AxialTree& workstring );

  AxialTree interpret(AxialTree& workstring);

  // interpret the string and plot it module by module
  void stepInterpretation(AxialTree& workstring);

  /** derive */
  inline AxialTree derive( )
  { return derive(__axiom, 0,__max_derivation); }
  inline AxialTree derive( size_t nb_iter )
  { return derive(__axiom, 0, nb_iter); }

  inline AxialTree derive( const AxialTree& workstring )
  { return derive(workstring, 0,__max_derivation); }

  inline AxialTree derive( const AxialTree& workstring, size_t nb_iter )
  { return derive(workstring, 0,nb_iter ); }

  AxialTree derive( const AxialTree& workstring, 
                     size_t starting_iter , 
                     size_t nb_iter , 
                     bool previouslyinterpreted = false  );

  AxialTree partial_derivation(AxialTree& workingstring,
                                 size_t beg, 
                                 size_t size);

  AxialTree decompose( const AxialTree& workstring  );

  /** Animation */
  inline AxialTree animate()
  { return animate(__context.get_animation_timestep(),0,__max_derivation); }
  inline AxialTree animate(double dt)
  { return animate(dt,0,__max_derivation); }
  inline AxialTree animate(double dt, size_t order)
  { return animate(dt,0,order); }
  inline AxialTree animate(double dt, size_t beg, size_t nb_iter)
  { return animate(__axiom,dt,beg,nb_iter); }
  AxialTree animate(const AxialTree& workstring, double, size_t beg, size_t nb_iter);

  /** Record */
  inline void record(const std::string& prefix, const std::string& suffix = "png")
  { record(prefix,__axiom,0,__max_derivation, suffix); }
  inline void record(const std::string& prefix, size_t nb_iter, const std::string& suffix = "png")
  { record(prefix,__axiom,0,nb_iter,suffix); }
  inline void record(const std::string& prefix, size_t beg, size_t nb_iter, const std::string& suffix = "png")
  { return record(prefix,__axiom,beg,nb_iter,suffix); }
  void record(const std::string& prefix, const AxialTree& workstring, size_t beg, size_t nb_iter, const std::string& suffix = "png");

  /** nb of iterations */
  inline size_t derivationLength() const { return __max_derivation; }
  inline void setDerivationLength(size_t v) { __max_derivation = v; }

  inline size_t decompositionMaxDepth() const { return __decomposition_max_depth; }
  inline void setDecompositionMaxDepth(size_t v) { __decomposition_max_depth = v; }

  inline size_t interpretationMaxDepth() const { return __interpretation_max_depth; }
  inline void setInterpretationMaxDepth(size_t v) { __interpretation_max_depth = v; }

  /** context */
  inline LsysContext * context() { return &__context; } 
  inline const LsysContext * context() const { return &__context; } 
  /** make current or disable the context of self */
  inline void makeCurrent() { return __context.makeCurrent(); }
  inline bool isCurrent() const { return __context.isCurrent(); }
  inline void done() { return __context.done(); }

  /** direction of iteration */
  inline eDirection getDirection() const { return __context.getDirection(); }

  /** early return */
  void enableEarlyReturn(bool val) ;
  bool isEarlyReturnEnabled() ;

  /* last computation result */
  size_t getLastIterationNb() { return __context.getIterationNb(); }
  PGL(ScenePtr) getLastComputedScene() { return __lastcomputedscene; }

   /** test if self is actually iterating */
   bool isRunning() const;
   /** force release of running state in case of violent ending **/
   void forceRelease();

   void addSubLsystem(const std::string& lfile);
   void addSubLsystem(const Lsystem& sublsystem);

   class LPY_API Debugger : public PGL_NAMESPACE_NAME::RefCountObject {
   public:
	   Debugger() : alwaysStop(true) { }
	   virtual ~Debugger() ;

	   virtual void begin(const AxialTree& src, eDirection) { }
	   virtual void end(const AxialTree& result) { }
	   virtual void total_match( AxialTree::const_iterator match_beg, 
							     AxialTree::const_iterator match_end,
							     const AxialTree& dest, size_t prodlength,
							     const LsysRule *,
							     const ArgList) { }

	   virtual void partial_match(AxialTree::const_iterator match_beg, 
								  AxialTree::const_iterator match_end,
								  const AxialTree& dest, const LsysRule *,
								  const ArgList) { }

	   virtual bool error_match(AxialTree::const_iterator match_beg, 
								AxialTree::const_iterator match_end,
								const AxialTree& dest, const LsysRule *,
								const ArgList) { return false; }

	   virtual void identity(AxialTree::const_iterator match_pos, 
							 const AxialTree& dest) { }

	   virtual bool shouldStop(AxialTree::const_iterator match_beg,
							   AxialTree::const_iterator match_end,
							   const LsysRule * rule) const
	   { if (alwaysStop) return true;
		 else return codeHasBreakPointAt(rule) || 
			         lstringHasBreakPointAt(match_beg,match_end); }

	   virtual bool shouldStop(AxialTree::const_iterator match) const
	   { if (alwaysStop) return true;
		 else return lstringHasBreakPointAt(match); }
	   
	   inline bool codeHasBreakPointAt(const LsysRule * rule) const {
		   for(LineBreakSet::const_iterator it = linebreaks.begin(); it != linebreaks.end(); ++it){
			   int l = *it - rule->lineno;
			   if(l >= 0 && l < rule->getCodeLength()) return true;
		   }
		   return false;
	   }

	   inline bool lstringHasBreakPointAt(AxialTree::const_iterator match_beg,
										  AxialTree::const_iterator match_end) const
	   {
		   for(LstringBreakSet::const_iterator it = lstringbreaks.begin(); it != lstringbreaks.end(); ++it){
			   for(AxialTree::const_iterator it2 = match_beg; it2 !=  match_end; ++it2)
				   if(*it == it2) return true;
		   }
		   return false;
	   }

	   inline bool lstringHasBreakPointAt(AxialTree::const_iterator match) const
	   {
		   for(LstringBreakSet::const_iterator it = lstringbreaks.begin(); it != lstringbreaks.end(); ++it){
				   if(*it == match) return true;
		   }
		   return false;
	   }

	   inline void insertCodeBreakPointAt(uint32_t breakpoint) 
	   { linebreaks.push_back(breakpoint); }

	   inline void removeCodeBreakPointAt(uint32_t breakpoint) 
	   { 
		   LineBreakSet::iterator it =  std::find(linebreaks.begin(),linebreaks.end(),breakpoint);
		   if (it == linebreaks.end())LsysError("Cannot remove code break point.");
		   else linebreaks.erase(it); 
	   }
	   inline bool hasCodeBreakPointAt(uint32_t breakpoint) const 
	   { 
		   LineBreakSet::const_iterator it = std::find(linebreaks.begin(),linebreaks.end(),breakpoint);
		   return it != linebreaks.end(); 
	   }

	   inline void insertLstringBreakPointAt(AxialTree::const_iterator breakpoint) { lstringbreaks.push_back(breakpoint); }
	   inline void removeLstringBreakPointAt(AxialTree::const_iterator breakpoint) {
		   LstringBreakSet::iterator it = std::find(lstringbreaks.begin(),lstringbreaks.end(),breakpoint);
		   if (it == lstringbreaks.end())LsysError("Cannot remove lstring break point.");
		   else lstringbreaks.erase(it); 

	   }
	   inline bool hasLstringBreakPointAt(AxialTree::const_iterator breakpoint) const
	   { 
		   LstringBreakSet::const_iterator it = std::find(lstringbreaks.begin(),lstringbreaks.end(),breakpoint);
		   return it != lstringbreaks.end(); 
	   }

	   inline void clearBreakPoints() 
	   { clearCodeBreakPoints();clearLstringBreakPoints(); }
	   inline void clearCodeBreakPoints() { linebreaks.clear(); }
	   inline void clearLstringBreakPoints() { lstringbreaks.clear(); }
		

	   bool alwaysStop;
	   typedef std::vector<uint32_t> LineBreakSet;
	   LineBreakSet linebreaks;
	   typedef std::vector<AxialTree::const_iterator> LstringBreakSet;
	   LstringBreakSet lstringbreaks;
   };
   typedef RCPtr<Debugger> DebuggerPtr;

   inline void setDebugger(DebuggerPtr debugger) { __debugger = debugger; }
   inline DebuggerPtr getDebugger() const { return __debugger; }
   inline bool hasDebugger() const { return is_valid_ptr(__debugger); }
   inline void clearDebugger() { __debugger = DebuggerPtr(); }

   pgl_hash_map_string<std::string> get_rule_fonction_table() const;

protected:

  struct RuleGroup {
    RuleGroup();

    const RuleSet& getGroup(eRuleType) const;
    RuleSet& getGroup(eRuleType);
    bool hasQuery(eRuleType) const;

    RuleSet production;
    RuleSet decomposition;
    RuleSet interpretation;
    bool __prodhasquery;
    bool __dechasquery;
    bool __inthasquery;
  };

  typedef std::vector<RuleGroup> RuleGroupList;


  LsysRule& __addRule( const std::string& rule, int type, size_t group,  int lineno = -1, const ConsiderFilterPtr filter = ConsiderFilterPtr() );
  LsysRule& __addProductionRule( const std::string& rule, size_t group,  int lineno = -1, const ConsiderFilterPtr filter = ConsiderFilterPtr() );
  LsysRule& __addDecompositionRule( const std::string& rule, size_t group,  int lineno = -1, const ConsiderFilterPtr filter = ConsiderFilterPtr() );
  LsysRule& __addInterpretationRule( const std::string& rule, size_t group,  int lineno = -1, const ConsiderFilterPtr filter = ConsiderFilterPtr() );

 void __clear();
 void __importPyFunctions();

 Lsystem(const Lsystem& lsys);
 Lsystem& operator=(const Lsystem& lsys);

 AxialTree __homomorphism(AxialTree& workstring);
 void __plot(AxialTree& workstring, bool checkLastComputedScene =  false);
 void __turtle_interpretation(AxialTree& workstring, PGL::Turtle& t, boost::python::object pyturtle = boost::python::object());
 AxialTree __derive( size_t starting_iter , 
                      size_t nb_iter , 
                      const AxialTree& workstring, 
                      bool previouslyinterpreted = false);

 AxialTree __decompose( const AxialTree& workstring );

 AxialTree __step(AxialTree& workingstring,
                   const RulePtrMap& ruleset,
                   bool query,bool& matching,
                   eDirection direction);
 
 AxialTree __parallelStep(AxialTree& workingstring,
                   const RulePtrMap& ruleset,
                   bool query,bool& matching,
                   eDirection direction);

 AxialTree __debugStep(AxialTree& workingstring, const RulePtrMap& ruleset,
					bool query, bool& matching, eDirection direction, Debugger& debugger);

 AxialTree __stepWithMatching(AxialTree& workingstring,
				              const RulePtrMap& ruleset,
				              bool query,
                              StringMatching& matching);
 AxialTree __recursiveSteps(AxialTree& workingstring,
				            const RulePtrMap& ruleset, 
                            size_t maxdepth, bool& matching);

 void __recursiveInterpretation(AxialTree& workingstring,
				                const RulePtrMap& ruleset,
                                PGL::Turtle& turtle,
                                boost::python::object& pyturtle,
                                size_t maxdepth);

 void __recursiveStepInterpretation(AxialTree& workingstring,
				                const RulePtrMap& ruleset,
                                PGL::PglTurtle& turtle,
                                boost::python::object& pyturtle,
                                size_t maxdepth);

 template<class Interpreter>
 void __gRecursiveInterpretation(AxialTree& workingstring,
				                const RulePtrMap& ruleset,
                                Interpreter& interpreter,
                                size_t maxdepth,
								bool withid = true);

 RulePtrMap __getRules(eRuleType type, size_t group, eDirection direction, bool * hasQuery = NULL);

 void __apply_pre_process(AxialTree& workstring, bool starteach = true);
 PGL(ScenePtr) __apply_post_process(AxialTree& workstring, bool endeach = true);

  AxialTree __axiom;
  RuleGroupList __rules;

  size_t __max_derivation;
  size_t __decomposition_max_depth;
  size_t __interpretation_max_depth;
  size_t __currentGroup;
  LocalContext __context;
  PGL(ScenePtr) __lastcomputedscene;
  
  RuleGroup& __group(size_t group) ;
  const RuleGroup& __group(size_t group) const;

  DebuggerPtr __debugger;
  bool __newrules;

private:
#ifdef MULTI_THREADED_LSYSTEM
  void acquire() const;
  void release() const;
  class LsysAcquirer {
    public:
      const Lsystem * __lsys;
      LsysAcquirer(const Lsystem * lsys) ;
      ~LsysAcquirer() ;
  };
  friend class LsysAcquirer;

  struct LsysRessource {
      LsysRessource();
      QMutex mutex;
  };
  LsysRessource * __ressource;
#endif

};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

#endif
