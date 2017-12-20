import os
pj=os.path.join

PREFIX = os.path.abspath(os.environ.get('PREFIX'))
SRC_DIR = os.environ.get('SRC_DIR')

vplants_plantgl_include = pj(PREFIX,'include')
vplants_plantgl_lib = pj(PREFIX,'lib')

if 'CPU_COUNT' in os.environ:
    num_jobs = os.environ['CPU_COUNT']

