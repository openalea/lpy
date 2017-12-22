# -*-python-*-

from openalea.sconsx import config, environ
import os

ALEASolution = config.ALEASolution
pj= os.path.join

name='lpy'

options = Variables(['../options.py', 'options.py'], ARGUMENTS )
options.Add(EnumVariable('QT_VERSION','Qt major version to use','4',allowed_values=('4','5')))

qt_env = Environment(options=options, tools=[])
qt_version = eval(qt_env['QT_VERSION'])

tools = ['boost_python', 'vplants.plantgl','qt'+str(qt_version)]

env = ALEASolution(options, tools)
env.Append( CPPPATH = pj( '$build_includedir','lpy' ) )


SetOption('implicit_cache', 0)

# Build stage
prefix= env['build_prefix']

SConscript( pj(prefix,"src/cpp/SConscript"),
            exports={"env":env} )

SConscript( pj(prefix,"src/wrapper/SConscript"),
            exports={"env":env} )

Default("build")

def generate_qtbuilddir():
    standartprefix = 'build-scons'
    if os.path.basename(prefix) != standartprefix:
        if os.path.exists(standartprefix):
            if os.path.isdir(standartprefix) and not os.path.islink(standartprefix): 
                import shutil
                shutil.rmtree(standartprefix)
            else: os.remove(standartprefix)
        os.symlink(prefix, standartprefix)

if os.name == 'posix' and qt_version and not 'CONDA_BUILD' in os.environ:
    generate_qtbuilddir()