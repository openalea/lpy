import xsimlab as xs
from paramtable import ParamTable, ArrayParameterSet
import openalea.lpy as lpy
import numpy as np
predefined_variables = ['nsteps', 'step', 'step_start', 'step_delta']

def get_caller_namespace():
    """ Retrieve namespace of the caller function to insert it elements """
    import inspect
    return inspect.getouterframes(inspect.currentframe())[2][0].f_locals
 
def gen_param_structs(modules, process):
    """ Generate Parameter structure, based on ArrayParameterSet,  for the given modules """
    return dict([(module+'Params', type(module+'Params', (ArrayParameterSet,), {'table' : ParamTable(process, module) })) for module in modules])

class AbstractLpyProcess:

    #lscene = xs.any_object()
    #lstring  = xs.any_object()
    lpyfile = None
    graphicalparameters = None
    globaldependencies = {} 

    def initialize(self):
            #for name, pnames in self.modules.items():
            #    for pname in pnames:
            #        setattr(self, name+'_'+pname, np.array([], dtype=float))
        parameters = { 'process': self }
        for n in self.externs:
            parameters[n]=getattr(self, n)
        parameters.update(gen_param_structs(self.modules, self))
        if not self.graphicalparameters is None:
            code = open(self.lpyfile,'r').read()
            self.lsystem = lpy.Lsystem()
            self.lsystem.set(code+self.graphicalparameters.generate_py_code(), parameters)
        else:
            self.lsystem = lpy.Lsystem(self.lpyfile, parameters)
        self.lstring = self.lsystem.axiom
        self.scene = self.lsystem.sceneInterpretation(self.lstring)

    @xs.runtime(args=predefined_variables)
    def run_step(self, nsteps, step, step_start, step_delta):
        """ A run step function to run an lsystem """
        parameters = {}
        for n in predefined_variables:
            parameters[n]=locals()[n]
        # Do we need to set it before each iteration ?
        for n in self.externs:
            parameters[n]=getattr(self, n)
        self.lsystem.execContext().updateNamespace(parameters)
        self.lstring = self.lsystem.derive(self.lstring, 1)
        self.lscene = self.lsystem.sceneInterpretation(self.lstring)

    @classmethod
    def init_vars(clss, initproperties):
        for pname, dep in clss.globaldependencies.items():
            v = initproperties.get(dep,{})
            v[pname] = np.array([], dtype=float)
            initproperties[dep] = v
        return initproperties



def parse_extern_modules(lpyfile):
    """ Parse an lpyfile to retrieve its modules and expected extern parameters """
    lines = list(open(lpyfile).readlines())
    externs = set()
    def f(**kwd):
        nonlocal externs
        externs = externs.union(set(kwd.keys()))
    code = ''.join([l for l in lines if l.startswith('extern')])
    n = {'extern' : f, 'externs' : externs}
    exec(code, n, n)
    
            
    code2 = ''.join([l for l in lines if l.startswith('module')])
    from openalea.lpy import Lsystem
    l = Lsystem()
    l.setCode(code2)
    l.makeCurrent()
    modules = {}
    for m in l.execContext().declaredModules():
        modules[m.name] = m.parameterNames
    l.done()
    return externs, modules

def retrieve_extern_modules(lsystem):
    externs = set(getattr(lsystem, '__extern__',set()))
    lsystem.makeCurrent()
    modules = {}
    for m in lsystem.execContext().declaredModules():
        modules[m.name] = m.parameterNames
    lsystem.done()
    return externs, modules
        
def gen_properties(externs, modules, modulestoconsider = None, globaldependencies = {}, propertymapping = {}):
    """ Generate the properties of the xs Process class that will run the lpyfile """
    import numpy as np
    externs = externs.difference(predefined_variables)
    if not modulestoconsider is None:
        mmodules = dict()
        for name, pnames in modules.items():
            if name in modulestoconsider:
                mmodules[name] = pnames
        modules = mmodules
    properties = {}
    properties['modules'] = modules
    for m, v in modules.items():
        properties[m] = xs.index(dims=m)
        for p in v:
            pname = m+'_'+p
            properties[pname] = propertymapping.get(pname,
                                    xs.global_ref(pname, intent='in')
                                    if pname in globaldependencies else
                                    xs.variable( dims=m, intent='out', encoding={'dtype': np.float}))
    properties['globaldependencies'] = globaldependencies
    properties['externs'] = externs
    for e in externs:
        properties[e] = xs.variable()
    return properties


def xs_lpyprocess(name, lpyfile,  graphicalparameters = None, modulestoconsider = None, globaldependencies = {}, propertymapping = {}):
    """ Generate the xs process class under the given name with adequate properties from the lpy file. """
    externs, modules = parse_extern_modules(lpyfile)

    properties = gen_properties(externs, modules, modulestoconsider, globaldependencies, propertymapping)
    properties['lpyfile']  = lpyfile
    properties['graphicalparameters']  = graphicalparameters
    properties['lscene']  = xs.any_object()
    properties['lstring']  = xs.any_object()
    process = xs.process(type(name, (AbstractLpyProcess,), properties))
    get_caller_namespace()[name] = process
    return process

def xs_lpydisplay_hook(processname, scale = 1, delay = 0.02):
    """ Generate a hook to display the lpy simulation from process given by processname """
    import pgljupyter
    from IPython.display import display
    from xsimlab.monitoring import ProgressBar
    from openalea.plantgl.all import Sequencer

    view = pgljupyter.SceneWidget()
    display(view)
    s = Sequencer(delay)

    @xs.runtime_hook('run_step')
    def hook(model, context, state):
        s.touch()
        view.set_scenes([state[('devel', 'lscene')]], scales=scale)


    return [hook, ProgressBar()]

__all__ = ['xs_lpyprocess', 'xs_lpydisplay_hook']
