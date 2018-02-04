import os

if 'CPU_COUNT' in os.environ:
    num_jobs = os.environ['CPU_COUNT']

compiler = 'msvc'
qhull_libs_suffix = 'static'
boost_libs_suffix = ''
