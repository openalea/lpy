# -*-python-*-

from openalea.sconsx import config, environ
import os

ALEASolution = config.ALEASolution
pj= os.path.join

name='pylsystems'

options = Variables(['../options.py', 'options.py'], ARGUMENTS )
options.Add(EnumVariable('QT_VERSION','Qt major version to use','4',allowed_values=('4','5')))

qt_env = Environment(options=options, tools=[])
qt_version = int(qt_env['QT_VERSION'])

tools = ['boost_python', 'vplants.plantgl','qt'+str(qt_version)]

env = ALEASolution(options, tools)
env.Append( CPPPATH = pj( '$build_includedir','lpy' ) )

# Build stage
prefix= env['build_prefix']

SConscript( pj(prefix,"src/cpp/SConscript"),
            exports={"env":env} )

SConscript( pj(prefix,"src/wrapper/SConscript"),
            exports={"env":env} )

Default("build")
