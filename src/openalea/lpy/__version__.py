__version_number__ = 0x030800
__revision_str__=""


def get_major(version): 
    return ((__version_number__ & 0xff0000) >> 16)

def get_minor(version): 
    return ((__version_number__ & 0x00ff00) >> 8)

def get_revision(version): 
    return (__version_number__ & 0x0000ff)

def get_version_majorminor(version):
    return float(str(get_major(version))+'.'+str(get_minor(version)))

def version_string(version, gitrev = ''):
    num_version_str = str(get_major(version))+'.'+str(get_minor(version))+'.'+str(get_revision(version))
    if len(gitrev) > 0:
        gitrev = gitrev.split(':')[1][1:-1]
        num_version_str = num_version_str+'-r'+gitrev
    return num_version_str


LPY_VERSION_MAJOR = get_major(__version_number__)
LPY_VERSION_MINOR = get_minor(__version_number__)
LPY_VERSION_REV   = get_revision(__version_number__)
LPY_VERSION_STR   = version_string(__version_number__, __revision_str__)