# -*- coding: iso-8859-15 -*-

# Header

import os, sys
pj = os.path.join

f = pj(os.path.dirname(__file__),'src', 'openalea', 'lpy','__version__.py')
d = {}
execfile(f,d,d)
version = d['LPY_NUM_VERSION_STR']

py2exe_file = pj(os.path.dirname(__file__),'src', 'openalea', 'lpy','gui','py2exe_release.py')
if not os.path.exists(py2exe_file):
    f = file(py2exe_file,'w')
    f.close()

##############
# Setup script

# Package name
name = 'lpy'
namespace = 'openalea'
pkg_name= namespace + '.' + name

print pkg_name,': version =',version

description= 'Lindenmayer Systems in Python package for OpenAlea.' 
long_description= '''
A Python version of Lindenmayer Systems.
Based on Dr. Prusinkiewicz et al. cpfg-lpfg specifications.
Implemented by the Virtual Plants team.'''

# Author
author= 'Frederic Boudon'
author_email= 'frederic.boudon@cirad.fr'

url= 'http://openalea.gforge.inria.fr/dokuwiki/doku.php?id=packages:vplants:lpy:main'

# LGPL compatible INRIA license
license= 'Cecill V2' 

# Scons build directory
build_prefix= "build-scons"


libdirs = pj(build_prefix,'lib')+' '+pj('../PlantGL',build_prefix,'lib')
print libdirs

from setuptools import setup

MainScript = 'src/openalea/lpy/gui/lpy.pyw'

import sys
if sys.platform =='darwin':
  import py2app
  option_name = 'py2app'
  extra_options = { 'argv_emulation' : True, 
                    'compressed'     : False,
                    'optimize'       : 0,
                    # 'iconfile'       : 'lpy.icns',
                    'excludes' : [],
                    }
  builderoptions = {'app' : [MainScript]}
  build_prefix = 'build-scons'
else:
  import py2exe
  option_name = 'py2exe'
  extra_options =  { "dll_excludes" : ['MSVCP80.dll','MSVCR80.dll'] }
  builderoptions = {'windows' : [{'script' : MainScript, 
                                 'icon_resources' : [(1, "src/openalea/lpy/gui/logo.ico")] }] }
  build_prefix = ''

import glob
from os.path import splitext,basename
plugins = [splitext(basename(i))[0] for i in glob.glob('src/openalea/lpy/gui/plugins/[a-zA-Z0-9]*.py')]
print plugins

#"sip","stat","PyQt4.QtXml","distutils.util","ctypes", 
                                           #"ctypes.util","random",'IPython','IPython.frontend.qt','IPython.frontend.qt.console',
                                           #'IPython.kernel','IPython.kernel.inprocess','IPython.core','IPython.kernel.zmq','IPython.utils'
                                           #'IPython.external','IPython.qt','pygments.styles']+
                                          

goptions = { option_name : 
                {
                    'includes' : map(lambda x : '.'.join(['openalea.lpy.gui.plugins',x]), plugins) ,
                    #'packages' : ['sip','stat','PyQt4', 'distutils', 'ctypes', 'random', 'IPython', 'pygments', 'PIL','PyOpenGL']
                    'packages' : ['IPython','PyQt4','pygments','PIL','OpenGL']

                }
            }
goptions[option_name].update(extra_options)
print goptions


setup(
    name="Lpy",
    version=version,
    description=description,
    long_description=long_description,
    author=author,
    author_email=author_email,
    url=url,
    license=license,
    
    #scons_scripts = ['SConstruct'],
    #scons_parameters = ["build_prefix="+build_prefix],
    
    namespace_packages = [namespace],
    create_namespaces = True,
    
    # pure python  packages
    packages = [ pkg_name, pkg_name+'.gui', pkg_name+'.gui.plugins', pkg_name+'.cpfg_compat', 
                 'openalea.plantgl', 'openalea.vpltk', 'openalea.vpltk.qt', 'openalea.vpltk.shell', 'openalea.vpltk.check'
                ]+map(lambda x : '.'.join(['openalea.plantgl',x]),['math','scenegraph','algo','gui','codec']),
    py_modules = ['lpygui_postinstall'],

    # python packages directory
    package_dir = { '' : 'src','openalea.plantgl' : '../PlantGL/src/plantgl', 'openalea.mtg' : '../newmtg/src/mtg', 
    				     'openalea.vpltk' : '../../../openalea/vpltk/src/openalea/vpltk'},
                   
    # Add package platform libraries if any
    include_package_data = True,
    package_data = {'' : ['*.pyd', '*.so', '*.lpy', '*.dylib'],},
    zip_safe = False,

    # Specific options of openalea.deploy
    #lib_dirs = { 'lib' : libdirs},
    lib_dirs = { 'lib' : pj(build_prefix,'lib'), 'lib2' : pj('../PlantGL',build_prefix,'lib') },
    bin_dirs = { 'bin' : pj(build_prefix,'bin'), 'bin2' : pj('../PlantGL',build_prefix,'bin')},
    #inc_dirs = {'include' : pj(build_prefix, 'include') },
    share_dirs = {'share' : 'share', },

    # Dependencies
    # entry_points
    #entry_points = {
    #    "wralea": ["lpy = openalea.lpy.wralea",],
    #    'gui_scripts': ['lpy = openalea.lpy.gui.lpystudio:main',]
    #    },
    
    #postinstall_scripts = ['lpygui_postinstall'],
    
    # Dependencies
    setup_requires = ['openalea.deploy'],
    dependency_links = ['http://openalea.gforge.inria.fr/pi'],

    # py2exe or py2app options
    options=goptions,

    **builderoptions

    )


if  os.path.exists(py2exe_file):
    os.remove(py2exe_file)
    py2exe_cfile = py2exe_file.replace('.py','.pyc')
    if  os.path.exists(py2exe_cfile):
        os.remove(py2exe_cfile)
