# -*-python-*-

from openalea.sconsx import config, environ
import os, fnmatch

Config= config.Config
ALEAConfig= config.ALEAConfig
ALEAEnvironment= config.ALEAEnvironment

pj= os.path.join

name='pylsystems'

SConsignFile()

options = Options(['../options.py', 'options.py'], ARGUMENTS )
prj_conf= ALEAConfig(name,['boost_python', 'plantgl','qt4'])

# Set all the common options for the package
prj_conf.UpdateOptions( options )

prj_env= Environment( options= options )
prj_conf.Update( prj_env )

# Generate Help available with the cmd scons -h
Help(options.GenerateHelpText(prj_env))

# Set build directory
prefix= prj_env['build_prefix']
BuildDir( prefix, '.' )

prj_env.Prepend(LIBPATH='$build_libdir')
prj_env.Append( CPPPATH = pj( '$build_includedir','lpy' ) )

# Build stage
SConscript( pj(prefix,"src/cpp/SConscript"),
            exports={"env":prj_env} )

SConscript( pj(prefix,"src/wrapper/SConscript"),
            exports={"env":prj_env} )


Default("build")
