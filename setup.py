# -*- coding: iso-8859-15 -*-
# Header

from openalea.deploy.metainfo import read_metainfo
metadata = read_metainfo('metainfo.ini', verbose=True)
for key,value in zip(metadata.keys(), metadata.values()):
    exec("%s = '%s'" % (key, value))



import os, sys
pj = os.path.join

f = pj(os.path.dirname(__file__),'src', 'openalea', 'lpy','__version__.py')
d = {}
execfile(f,d,d)

    
##############
# Setup script

# Package name
pkg_name= namespace + '.' + package
wralea_name= namespace + '.' + package + '_wralea'

version= d['LPY_NUM_VERSION_STR']
print pkg_name,': version =',version


# Scons build directory
build_prefix= "build-scons"

from setuptools import setup
from openalea.deploy.binary_deps import binary_deps

install_requires = [binary_deps('vplants.plantgl')]
if 'linux' not in sys.platform:
    install_requires.append('PyOpenGL')
    install_requires.append('pyqglviewer')

setup(
    name=name,
    version=version,
    description=description,
    long_description=long_description,
    author=authors,
    author_email=authors_email,
    url=url,
    license=license,
    
    scons_scripts = ['SConstruct'],
    scons_parameters = ["build_prefix="+build_prefix],
    
    namespace_packages = [namespace],
    create_namespaces = True,
    
    # pure python  packages
    packages = [ pkg_name, pkg_name+'.gui', wralea_name ],
    py_modules = ['lpygui_postinstall'],

    # python packages directory
    package_dir = { '' : 'src',},
                   
    # Add package platform libraries if any
    include_package_data = True,
    package_data = {'' : ['*.pyd', '*.so', '*.dylib', '*.lpy','*.ui','*.qrc'],},
    zip_safe = False,

    # Specific options of openalea.deploy
    lib_dirs = {'lib' : pj(build_prefix, 'lib'),},
    bin_dirs = {'bin':  pj(build_prefix, 'bin'),},
    inc_dirs = {'include' : pj(build_prefix, 'include') },
    share_dirs = {'share' : 'share', },

    # Dependencies
    # entry_points
    entry_points = {
        "wralea": ["lpy = openalea.lpy_wralea",],
        'gui_scripts': ['lpy = openalea.lpy.gui.lpystudio:main',]
        },
    
    postinstall_scripts = ['lpygui_postinstall'],
    
    # Dependencies
    setup_requires = ['openalea.deploy'],
    dependency_links = ['http://openalea.gforge.inria.fr/pi'],
    install_requires = install_requires,

    pylint_packages = ['src/openalea/lpy/gui']
    )


