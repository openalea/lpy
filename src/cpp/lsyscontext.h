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

#ifndef __PGL_LSYSCONTEXT_H__
#define __PGL_LSYSCONTEXT_H__

#include "axialtree.h"
#include "lsysoptions.h"
#include <plantgl/algo/modelling/pglturtle.h>
#include <plantgl/tool/util_hashset.h>
#include <QtCore/QReadWriteLock>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class LPY_API LsysContext {
public:
  friend class Lsystem;

  /** string value of python variable containing lsystem informations. */
  static const std::string InitialisationFunctionName;
  static const std::string AxiomVariable;
  static const std::string DerivationLengthVariable;
  static const std::string DecompositionMaxDepthVariable;
  static const std::string HomomorphismMaxDepthVariable;
  static double DefaultAnimationTimeStep;

  /** Constructor */
  LsysContext();

  /** Destructor */
  virtual ~LsysContext();

  /** clear context. Set python namespace to default. Keep __builtin__, pylsystems and __filename__ object */
  void clear();
  /** Test whether namespace is empty */
  bool empty() const;

  /** consider and ignore of symbol*/
  void consider(const std::string& modules);
  void ignore(const std::string& modules);
  bool isConsidered(const std::string& module) const;
  bool isIgnored(const std::string& module) const;
  bool isConsidered(const Module& module) const;
  bool isIgnored(const Module& module) const;
  bool ignoring() const { return __ignore_method; }
  std::string keyword() const;

  /** string representation */
  std::string str() const ;

  /** The Start, End, StartEach and EndEach execution */
  void start();
  void end();
  void startEach();
  void endEach();
  void endEach(AxialTree&);
  void endEach(AxialTree&, const PGL::ScenePtr&);

  /** The Start, End, StartEach and EndEach initialisation */
  void setStart(boost::python::object func);
  void setEnd(boost::python::object func);
  void setStartEach(boost::python::object func);
  void setEndEach(boost::python::object func);
  void check_init_functions();
  size_t getEndEachNbArgs() const { return  __nbargs_of_endeach; }


  /// initialise context using python function in namespace.
  bool initialise();
  size_t initialiseFrom(const std::string& lcode);

  /** compilation of code into the python namespace */
  void execute(const std::string&)  ;
  boost::python::object 
	evaluate(const std::string&)  ;
  boost::python::object 
	try_evaluate(const std::string&)  ;
  virtual boost::python::object
	compile(const std::string& name, const std::string& code)  ;

  /** application of a function */
  void func(const std::string& funcname);

  /** access to value of the python namespace */
  int readInt(const std::string&) ;
  float readReal(const std::string&)  ;

  /** python namespace management */
  virtual void clearNamespace();
  virtual void updateNamespace(const boost::python::dict&);
  virtual void getNamespace(boost::python::dict&) const;

  /** access to python object of the namespace */
  virtual bool hasObject(const std::string& name) const;
  virtual boost::python::object getObject(const std::string& name) const;
  virtual void setObject(const std::string& name, 
				 const boost::python::object&);
  virtual void delObject(const std::string& name) ;
  bool copyObject(const std::string& name, LsysContext * sourceContext) ;
  
  /** make current or disable a context */
  void makeCurrent();
  bool isCurrent() const ;
  void done() ;

  /** static functions to access context */
  static LsysContext * currentContext();
  static LsysContext * globalContext();
  static LsysContext * defaultContext();
  static void cleanContexts();

  /** control of the direction of next iteration */
  void backward() { __direction = eBackward; }
  void forward() { __direction = eForward; }
  bool isForward() { return __direction == eForward; }
  eDirection getDirection() const { return __direction; }

  /** selection of group of rules */
  void useGroup(size_t gid) { __group = gid; }
  size_t getGroup() const  { return __group; }

  /** iterative production */
  void nproduce(const AxialTree& prod);
  void nproduce(const boost::python::list& prod);
  void reset_nproduction();
  AxialTree get_nproduction() const { return __nproduction; }

  /** animation time step property */
  double get_animation_timestep();
  void set_animation_timestep(double value);
  bool is_animation_timestep_to_default();

  /** Specify if the selection check is required */
  bool isSelectionRequired() const;
  void setSelectionRequired(bool enabled);

  /** Turtles and interpretation structures */
  PGL(PglTurtle) turtle;
  PGL(Turtle)    envturtle;

  /** Context options */
  LsysOptions options;

  /** module declaration. */
  void declare(const std::string& modules);
  void declare(ModuleClassPtr module);
  void undeclare(const std::string& modules);
  void undeclare(ModuleClassPtr module);
  bool isDeclared(const std::string& module);
  bool isDeclared(ModuleClassPtr module);
  ModuleClassList declaredModules() const { return __modules; }


  /** Iteration number property. Only set by Lsystem. Access by all other. */
public:
  size_t getIterationNb();
protected:
  void setIterationNb(size_t) ;

protected:
  /// initialise context using python function in namespace.
  bool __initialise();
  size_t __initialiseFrom(const std::string& lcode);

  /** Event when context is made current, release, pushed or restore */
  virtual void currentEvent();
  virtual void doneEvent();
  virtual void pushedEvent(LsysContext * newEvent);
  virtual void restoreEvent(LsysContext * previousEvent);

  /// protected copy constructor.
  LsysContext(const LsysContext& lsys);
  LsysContext& operator=(const LsysContext& lsys);

  /// protected access to python namespace. To be redefined.
  virtual PyObject * Namespace()  const { return NULL; };

  /// init options
  void init_options();

  /// attributes for ignore and consider
  typedef pgl_hash_map<size_t,ModuleClassPtr> ModuleClassSet;
  ModuleClassSet __keyword;
  bool __ignore_method;

  /// attributes for module declaration
  ModuleClassList __modules;

  /// next iteration control
  eDirection __direction;
  size_t __group;
  /// iterative production
  AxialTree __nproduction;

  /// selection required property
  bool __selection_required;

  /// animation step property and its mutex
  double __animation_step;
  QReadWriteLock __animation_step_mutex;

  /// iteration nb property and its mutex
  size_t __iteration_nb;
  QReadWriteLock __iteration_nb_lock;

  size_t __nbargs_of_endeach;

};

/*---------------------------------------------------------------------------*/

class LPY_API LocalContext : public LsysContext {
public:
  LocalContext(bool with_initialisation = true);
  ~LocalContext();

  boost::python::dict& getNamespace() { return __namespace; }
  const boost::python::dict& getNamespace() const { return __namespace; }

  virtual void clearNamespace();
  virtual void updateNamespace(const boost::python::dict&);
  virtual void getNamespace(boost::python::dict&) const;

  virtual bool hasObject(const std::string& name) const;
  virtual boost::python::object getObject(const std::string& name) const;
  virtual void setObject(const std::string& name, 
				 const boost::python::object&);
  virtual void delObject(const std::string& name) ;

protected:
  void initialisation();
  virtual PyObject * Namespace() const ;

  boost::python::dict __namespace;
};

/*---------------------------------------------------------------------------*/

class LPY_API GlobalContext : public LsysContext {
public:
  GlobalContext();
  ~GlobalContext();

  virtual void clearNamespace();
  virtual void updateNamespace(const boost::python::dict&);
  virtual void getNamespace(boost::python::dict&) const;

  virtual bool hasObject(const std::string& name) const;
  virtual boost::python::object getObject(const std::string& name) const;
  virtual void setObject(const std::string& name, 
				 const boost::python::object&);
  virtual void delObject(const std::string& name) ;
  virtual boost::python::object
	compile(const std::string& name, const std::string& code)  ;

  static boost::python::object getFunctionRepr();
protected:
  virtual PyObject * Namespace() const ;

  boost::python::handle<> __namespace;
  boost::python::dict __local_namespace;
  static boost::python::object __reprFunc;

};

/*---------------------------------------------------------------------------*/

void LPY_API consider(const std::string& modules);
void LPY_API ignore(const std::string& modules);
void LPY_API nproduce(const AxialTree& prod);
void LPY_API nproduce(const boost::python::list& prod);
void LPY_API nproduce(const std::string& modules);
void LPY_API useGroup(size_t gid);
size_t LPY_API getGroup();
void LPY_API setSelectionRequired(bool enabled);
bool LPY_API isSelectionRequired();
size_t LPY_API getIterationNb();
void LPY_API declare(const std::string& modules);
void LPY_API undeclare(const std::string& modules);
void LPY_API isDeclared(const std::string& module);

/*---------------------------------------------------------------------------*/

struct ContextMaintainer {
    bool is_set;
    LsysContext * context;

    ContextMaintainer(LsysContext * _context) : 
        is_set(!_context->isCurrent()), context(_context)
    { 
		if (is_set) context->makeCurrent(); 
    }

    ~ContextMaintainer() { if (is_set) context->done();  }
};

LPY_END_NAMESPACE
/*---------------------------------------------------------------------------*/

#endif
