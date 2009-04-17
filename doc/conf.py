
# This import will 
#   - import sphinx ini in this directory
#   - import the common.ini in openalea/doc
#   - execute the statements in openalea/doc/common_conf.py
# you may overwrite some paramters found in common.ini here below

import sys
import os

sys.path.append(os.path.join(os.getcwd(), '../../../openalea/doc'))

from common_conf import *

# Overwrite extension if required
#extensions = [
#    'sphinx.ext.autodoc',
#    'sphinx.ext.doctest', 
#    'sphinx.ext.intersphinx',
#    'inheritance_diagram', 
#    'sphinx.ext.pngmath',
#    'sphinx.ext.todo', 
#    'numpydoc',
#    'phantom_import', 
#    'autosummary',
#    'sphinx.ext.coverage',
#    'only_directives'
#    ]


# to speed up compilation in development mode, uncomment this line
#intersphinx_mapping = {}
