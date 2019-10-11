# -*- coding: utf-8 -*-
__revision__ = "$Id$"

# Header
import os, sys
pj = os.path.join

from openalea.deploy.metainfo import read_metainfo
metadata = read_metainfo('metainfo.ini', verbose=True)
for key,value in metadata.items():
    exec("%s = '%s'" % (key, value))

##############
# Setup script

# Package name
pkg_name= namespace + '.' + package

meta_version = version
# check that meta version is updated
f = pj(os.path.dirname(__file__),'src', 'openalea', 'lpy','__version__.py')
d = {}
exec(compile(open(f, "rb").read(), f, 'exec'),d,d)
version= d['LPY_VERSION_STR']
if meta_version != version:
    print ('Warning:: Update the version in metainfo.ini !!')
print (pkg_name+': version ='+version)


# Scons build directory
build_prefix = "build-cmake"

from setuptools import setup
from openalea.deploy.binary_deps import binary_deps

def compile_interface():
    cwd = os.getcwd()
    os.chdir(pj('src','openalea','lpy','gui'))
    sys.path = ['']+sys.path
    import generate_ui
    os.chdir(cwd)

compile_interface()
install_requires = []

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
        pkg_name,
        pkg_name + '_d',
        pkg_name + '_wralea',
        pkg_name + '.gui',
        pkg_name + '.gui.plugins',
        pkg_name + '.cpfg_compat'
    ],

    # python packages directory
    package_dir = { '' : 'src',},

    # Add package platform libraries if any
    include_package_data = True,
    package_data = {'' : ['*.pyd', '*.so', '*.dylib', '*.lpy','*.ui','*.qrc'],},
    zip_safe = False,

    # Dependencies
    entry_points = {
        "wralea": ["lpy = openalea.lpy_wralea",],
        'gui_scripts': ['lpy = openalea.lpy.gui.lpystudio:main',],
        'console_scripts': ['cpfg2lpy = openalea.lpy.cpfg_compat.cpfg2lpy:main',],
    },

    # Dependencies
    setup_requires = ['openalea.deploy'],
    dependency_links = ['http://openalea.gforge.inria.fr/pi'],
    install_requires = install_requires,

    pylint_packages = ['src/openalea/lpy/gui']
)
