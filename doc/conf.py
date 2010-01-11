import os,sys

# read sphinx conf.py file
from openalea.misc.sphinx_configuration import *
from openalea.misc.sphinx_tools import sphinx_check_version
from openalea.deploy.metainfo import read_metainfo, compulsary_words

sphinx_check_version()                      # check that sphinx version is recent
metadata = read_metainfo('../metainfo.ini') # read metainfo from common file with setup.py
for key in compulsary_words:
    exec("%s = '%s'" % (key, metadata[key]))


# by product that need to be updated:
latex_documents = [('contents', 'main.tex', project + ' documentation', authors, 'manual')]

project = project + '.' + package
