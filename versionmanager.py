from openalea.sconsx.util.config_deploy import *

#################################################################

def from_py_variable(variable, fname):
        result = {}
        mglobals = {}
        execfile(fname, result, result)
        return HexVersion(result[variable])

def getLpyVersion():
    p = os.path.join('src', 'openalea', 'lpy', '__version__.py')
    return from_py_variable('__version_number__',p)

def getMetaInfo():
    metainfo = {}
    execfile(os.path.join('src','openalea','lpy','__init__.py'))
    return metainfo['__metainfo__']

def get_config_vars(env):
    from collections import OrderedDict

    result = OrderedDict()

    result['LPY_VERSION'] = getLpyVersion()
    result['LPY_VERSION_STR'] = result['LPY_VERSION'].to_string()

    qtversion = eval(env['QT_VERSION'])
    if qtversion is None:
        result['LPY_WITHOUT_QT'] = True
    else:
        result['LPY_WITHOUT_QT'] = False
        result['LPY_QT_VERSION'] = qtversion

    result = get_config_from_env(env, result, 'LPY')

    return result


def deployconfig(env):
    cconfig = get_config_vars(env)

    generate_config('L-Py', cconfig, 
                    pyconfigfname = os.path.join('src', 'openalea', 'lpy', 'config.py'),
                    cppconfigfname = os.path.join('src', 'cpp', 'userconfig.h'))

    generate_conda_config(cconfig['LPY_VERSION'])

#################################################################

if __name__ == '__main__':
    print (getLpyVersion().to_string())
