# -*-python-*-

from openalea.sconsx import config, environ
from openalea.sconsx.util.buildprefix import fix_custom_buildprefix
from openalea.sconsx.util.qt_check import detect_installed_qt_version
try:
    import openalea.plantgl.config as pglconf
except ImportError:
    pglconf = None

import os

ALEASolution = config.ALEASolution
pj= os.path.join

name='lpy'

options = Variables(['../options.py', 'options.py'], ARGUMENTS )
if pglconf :
    qt_version = pglconf.PGL_QT_VERSION
else:
    options.Add(EnumVariable('QT_VERSION','Qt major version to use',str(detect_installed_qt_version(4)),allowed_values=('4','5')))

    # Create an environment to access qt option values
    qt_env = Environment(options=options, tools=[])
    qt_version = eval(qt_env['QT_VERSION'])


tools = ['boost_python', 'openalea.plantgl','qt'+str(qt_version)]

env = ALEASolution(options, tools)
env.Append( CPPPATH = pj( '$build_includedir','lpy' ) )
env['QT_VERSION'] = str(qt_version)

# Build stage
prefix= env['build_prefix']


from versionmanager import deployconfig
# create config files
deployconfig(env)


SConscript( pj(prefix,"src/cpp/SConscript"), exports={"env":env} )

SConscript( pj(prefix,"src/wrapper/SConscript"), exports={"env":env} )

Default("build")

fix_custom_buildprefix(env)

