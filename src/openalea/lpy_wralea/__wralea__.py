from openalea.core import *
#from lpy_nodes import WithLpyGui

__name__ = "vplants.l-py"
__version__ = '0.0.1'
__license__ = 'CECILL-C'
__authors__ = 'Frederic Boudon'
__institutes__ =  'INRIA/CIRAD'
__description__ = "An L-system engine providing a python interface."
__url__ = 'http://openalea.gforge.inria.fr'

__all__ = []

widgetclass = "LSysWidget"
    
lsystem_node = Factory( name="LSystem", 
                   description="Build a LSystem object", 
                   category="scene.lsystem", 
                   nodemodule="lpy_nodes",
                   nodeclass="lsystem",
                   #widgetmodule="lpy_nodes",
                   #widgetclass = widgetclass,

                  inputs=(dict(name='Code', interface=ITextStr, value=''),
                          dict(name='Axiom', interface=ITextStr, value=''),
                          dict(name='DerivationLength', interface=IInt, value=-1),
                          dict(name='Parameters', interface=IDict, value={}),
                          ),
                  outputs=(dict(name='LSystem', interface=None),)
                  )
__all__.append('lsystem_node')


axialtree_node = Factory( name="AxialTree", 
                  description="Build an AxialTree object", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="axialtree",

                  inputs=(dict(name='LString', interface=ITextStr, value=''),),
                  outputs=(dict(name='AxialTree', interface=None),)
                  )
__all__.append('axialtree_node')


write_lstring = Factory( name="writeLstring", 
                  description="Write an axialtree object", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="writeLstring",

                  inputs=(dict(name='LString', interface=ITextStr, value=''),
                          dict(name='Filename', interface=IFileStr(save=True), value=''),
                          ),
                  outputs=()
                  )

__all__.append('write_lstring')


animate_node = Factory( name="animate", 
                  description="Animate a LSystem", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="animate",

                  inputs=(dict(name='LSystem', interface=None, value=None),
                          dict(name='TimeStep', interface=IInt, value=10),
                          ),
                  
                  outputs=()
                  )
__all__.append('animate_node')


run_node = Factory( name="run", 
                  description="Run a LSystem", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="run",

                  inputs=(dict(name='LSystem', interface=None, value=None),
                          dict(name='Axiom', interface=ITextStr, value=''),
                          dict(name='DerivationLength', interface=IInt, value=-1),
                          dict(name='Parameters', interface=IDict, value={}),
                          ),
                  
                  outputs=(dict(name='AxialTree', interface=ITextStr),
                           dict(name='LSystem'), )
                  )

__all__.append('run_node')

_iter_node = Factory( name="iterate", 
                  description="iterate over a LSystem", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="iterate",

                  inputs=(dict(name='LSystem', interface=None, value=None),
                          dict(name='Axiom', interface=ITextStr, value=''),
                          dict(name='DerivationLength', interface=IInt, value=-1),
                          dict(name='Parameters', interface=IDict, value={}),
                          dict(name='Local Parameters', interface=IDict, value={}),
                          ),
                  
                  outputs=(dict(name='AxialTree', interface=ITextStr),
                           dict(name='LSystem'),
                            dict(name='Parameters', interface=IDict),
                        )
                  )

__all__.append('_iter_node')

plot_node = Factory( name="plot", 
                  description="Plot a LSystem", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="plot",

                  inputs=(dict(name='Axiom', interface=ITextStr, value=''),
                          dict(name='LSystem', interface=None, value=None)
                          ),
                  
                  outputs=(dict(name='Axiom', interface=ITextStr),)
                  )
__all__.append('plot_node')

generate_scene = Factory( name="generateScene", 
                  description="Generate a scene from an AxialTree", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="Tree2Scene",

                  inputs=(dict(name='AxialTree', interface=ITextStr, value=''),
                          dict(name='LSystem', interface=None, value=None),
                          ),
                  
                  outputs=(dict(name='Scene', interface=None),)
                  )

__all__.append('generate_scene')

run_lpy = Factory( name="run lpy", 
                  description="Run a LSystem", 
                  category="scene.lsystem", 
                  nodemodule="lpy_nodes",
                  nodeclass="run_lpy",

                  inputs=(dict(name='LSystem file', interface=IFileStr),
                          dict(name='Axiom', interface=ITextStr, value=''),
                          dict(name='DerivationLength', interface=IInt, value=-1),
                          dict(name='Parameters', interface=IDict, value={}),
                          ),
                  
                  outputs=(dict(name='AxialTree', interface=ITextStr),
                           dict(name='LSystem'), )
                  )

__all__.append('run_lpy')

