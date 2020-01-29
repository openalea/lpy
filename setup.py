# -*- coding: utf-8 -*-
__revision__ = "$Id$"

# Header
import os, sys
pj = os.path.join

project = 'openalea'
package = 'lpy'
name = 'OpenAlea.Lpy'
namespace = 'openalea'
pkg_name = 'openalea.lpy'
description = 'Lindenmayer Systems in Python package for OpenAlea.'
long_description= 'L-Py is a simulation software that mixes L-systems construction with the Python high-level modeling language. '
authors = 'Frédéric Boudon'
authors_email = 'frederic.boudon@cirad.fr'
url= 'https://github.com/openalea/lpy'
# LGPL compatible INRIA license
license = 'Cecill-C'

##############
# Setup script

# Package name
pkg_name= namespace + '.' + package

# check that meta version is updated
lpydir = pj(os.path.dirname(__file__),'src', 'openalea', 'lpy')
versionfile = pj(lpydir,'__version__.py')

versioninfo = {}
with open(versionfile) as fp:
    exec(fp.read(), versioninfo)

version= versioninfo['LPY_VERSION_STR']
print (pkg_name+': version = '+version)


# cmake build directory
build_prefix = "build-cmake"

def compile_interface():
    cwd = os.getcwd()
    os.chdir(pj(lpydir,'gui'))
    sys.path = ['']+sys.path
    import generate_ui
    os.chdir(cwd)

    py2exe_file = pj(lpydir,'gui','py2exe_release.py')
    if not os.path.exists(py2exe_file):
        open(py2exe_file,'w').close()

if 'install' in sys.argv:
    compile_interface()


from setuptools import setup

setup(
    name=name,
    version=version,
    description=description,
    long_description=long_description,
    author=authors,
    author_email=authors_email,
    url=url,
    license=license,

    namespace_packages = [namespace],
    create_namespaces = False,

    # pure python  packages
    packages = [
        namespace,
        pkg_name,
        pkg_name + '_wralea',
        pkg_name + '.gui',
        pkg_name + '.gui.plugins',
        pkg_name + '.cpfg_compat'
    ],

    # python packages directory
    package_dir = { '' : 'src',},

    package_data={
        "": ["share/*","share/*/*","share/*/*/*","share/*/*/*/*", '*.pyd', '*.so', '*.dylib', '*.lpy','*.ui','*.qrc'],
    },

    # Add package platform libraries if any
    include_package_data = True,
    #package_data = {'' : ['*.pyd', '*.so', '*.dylib', '*.lpy','*.ui','*.qrc'],},
    zip_safe = False,

    # Dependencies
    entry_points = {
        "wralea": ["lpy = openalea.lpy_wralea",],
        'gui_scripts': ['lpy = openalea.lpy.gui.lpystudio:main',],
        'console_scripts': ['cpfg2lpy = openalea.lpy.cpfg_compat.cpfg2lpy:main',],
    }

)
