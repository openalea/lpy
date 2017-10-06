import os,sys
from os.path import join as pj

# read sphinx conf.py file
from openalea.misc.sphinx_configuration import *
from openalea.misc.sphinx_tools import sphinx_check_version
from openalea.deploy.metainfo import read_metainfo, compulsary_words

sphinx_check_version()                      # check that sphinx version is recent
metadata = read_metainfo('../metainfo.ini') # read metainfo from common file with setup.py
for key in compulsary_words:
    exec("%s = '%s'" % (key, metadata[key]))

f = pj(os.path.dirname(__file__),'..','src', 'openalea', 'lpy','__version__.py')
d = {}
execfile(f,d,d)
version= d['LPY_NUM_VERSION_STR']
release= d['LPY_RELEASE_STR']

# by product that need to be updated:
latex_documents = [('contents', 'main.tex', project + ' documentation', authors, 'manual')]

project = project + '.' + package
