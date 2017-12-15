import os

PREFIX = os.environ.get('PREFIX')
SRC_DIR = os.environ.get('SRC_DIR')

if 'CPU_COUNT' in os.environ:
    num_jobs = os.environ['CPU_COUNT']

