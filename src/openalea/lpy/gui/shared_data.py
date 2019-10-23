# -*- python -*-
#
#       OpenAlea.Deploy: OpenAlea setuptools extension
#
#       Copyright 2008 INRIA - CIRAD - INRA
#
#       File author(s): Thomas Cokelae
#
#       Distributed under the Cecill-C License.
#       See accompanying file LICENSE.txt or copy at
#           http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html
#
#       OpenAlea WebSite : http://openalea.gforge.inria.fr
#

"""INRIA GForge SOAP python API wrappers (based on SOAPpy)

"""
import types
import os
from os.path import join as pj
from os.path import realpath, isdir, isfile
try:
    from path import Path
except ImportError:
    try:
        from path import path as Path
    except ImportError:
        try:
            from openalea.core.path import path as Path
        except ImportError:
            from IPython.external.path import path as Path

__license__ = "Cecill-C"
__revision__ = " $Id: gforge.py 2243 2010-02-08 17:08:47Z cokelaer $ "

_share_path = pj('share', 'data')


def shared_data(package_path, filename=None, pattern=None,
                share_path=_share_path):
    """Return a valid pathname pointing to a shared-data directory or a shared-data file.

    :Parameters:
        - `package_path` (str or list) - Can be either a string representing a package path or a list of package paths. (e.g.
        :py:class:`path`('/home/user/openalea/deploy/src/openalea/deploy') or [:py:class:`path`('/usr/lib/pymodules/python2.7/numpy')]).
        If package_path is a list, then the first element of package_path is used.
        - `filename` (str) - An optional valid filename without any path that is expected
        to be found in :py:obj:`share_path`.
        - `share_path` (str) - The path where the share data directory is expected to be found.
        The default value is :py:const:`.share_path`. Important: All users should keep this
        default value in order to ease the share of data between the different packages.

    :Returns:
        a valid directory path if filename is not provided, and a valid file path to
        filename (including filename) otherwise. The directory or file is searched firstly into
        ':py:obj:`package_path`, then into ':py:obj:`package_path` parent directory, then
        into ':py:obj:`package_path` parent parent directory, and so on, going up until the parent parent
        directory of the last Python package found.
        If no valid path is found, returns None.

    :Returns Type:
        str

    :Examples:

    >>> shared_data(['/home/user/mypackage'])
    '/home/user/mypackage/share/data'
    >>> shared_data('/home/user/mypackage', 'my_file.csv')
    '/home/user/mypackage/share/data/my_file.csv'
    >>> shared_data(['/home/user/mypackage'], share_path='share/databases')
    '/home/user/mypackage/share/databases'
    >>> import mypackage
    >>> shared_data(mypackage, pattern='*.mtg')
    ['/home/user/mypackage/share/databases/mtg1.mtg', ...]
    """

    if isinstance(package_path, types.ModuleType):
        package_path = package_path.__path__

    if isinstance(package_path, list):
        if len(package_path) == 0:
            return None
        else:
            package_path = package_path[0]
    package_path = Path(package_path)
    ff = package_path / share_path
    ff = ff.realpath()
    shared_data_path = None
    if ff.isdir():
        if filename is None:
            shared_data_path = ff
            if pattern:
                l = ff.glob(pattern)
                if l:
                    shared_data_path = l
        else:
            ff = ff / filename
            ff = ff.realpath()
            if ff.isfile():
                shared_data_path = ff

    if shared_data_path is None and (package_path / '__init__.py').isfile():
        shared_data_path = shared_data(package_path.parent, filename, pattern,
                                       share_path)
        if shared_data_path is None:
            shared_data_path = shared_data(package_path.parent.parent, filename,
                                           pattern, share_path)

    return shared_data_path


# Backward compatibility
get_shared_data_path = shared_data
