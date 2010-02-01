# -*-python-*-

from openalea.sconsx import config, environ
import os

ALEASolution = config.ALEASolution
pj= os.path.join

name='pylsystems'

options = Variables(['../options.py', 'options.py'], ARGUMENTS )
tools = ['boost_python', 'vplants.plantgl','qt4']

env = ALEASolution(options, tools)
env.Append( CPPPATH = pj( '$build_includedir','lpy' ) )

# Build stage
prefix= env['build_prefix']

SConscript( pj(prefix,"src/cpp/SConscript"),
            exports={"env":env} )

SConscript( pj(prefix,"src/wrapper/SConscript"),
            exports={"env":env} )

Default("build")
