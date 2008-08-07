# -*- coding: iso-8859-15 -*-

# Header

import os, sys
pj = os.path.join

f = pj(os.path.dirname(__file__),'src', 'openalea', 'lpy','__version__.py')
d = {}
execfile(f,d,d)

##############
# Setup script

# Package name
name = 'lpy'
namespace = 'openalea'
pkg_name= namespace + '.' + name

version= str(d['LPY_VERSION_MAJOR'])+'.'+str(d['LPY_VERSION_MINOR'])+'.'+str(d['LPY_VERSION_REV'])
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

from setuptools import setup


setup(
    name="VPlants.Lpy",
    version=version,
    description=description,
    long_description=long_description,
    author=author,
    author_email=author_email,
    url=url,
    license=license,
    
    scons_scripts = ['SConstruct'],
    scons_parameters = ["build_prefix="+build_prefix],
    
    namespace_packages = [namespace],
    create_namespaces = True,
    
    # pure python  packages
    packages = [ pkg_name, pkg_name+'.gui', pkg_name+'.wralea' ],
    py_modules = ['lpygui_postinstall'],

    # python packages directory
    package_dir = { '' : 'src',},
                   
    # Add package platform libraries if any
    include_package_data = True,
    package_data = {'' : ['*.pyd', '*.so', '*.lpy'],},
    zip_safe = False,

    # Specific options of openalea.deploy
    lib_dirs = {'lib' : pj(build_prefix, 'lib'),},
    bin_dirs = {'bin':  pj(build_prefix, 'bin'),},
    inc_dirs = {'include' : pj(build_prefix, 'include') },
    share_dirs = {'share' : 'share', },

    # Dependencies
    # entry_points
    entry_points = {
        "wralea": ["lpy = openalea.lpy.wralea",],
        'gui_scripts': ['lpy = openalea.lpy.gui.lpystudio:main',]
        },
    
    postinstall_scripts = ['lpygui_postinstall'],
    
    # Dependencies
    setup_requires = ['openalea.deploy'],
    dependency_links = ['http://openalea.gforge.inria.fr/pi'],
    install_requires = ['PyOpenGL', 'plantgl'],

    )


