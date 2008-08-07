__version_number__ = 0x001302
__revision_str__="$Revision$"
__revision__ = __revision_str__.split(':')[1][1:-1]

LPY_VERSION_MAJOR = ((__version_number__ & 0xff0000) >> 16)
LPY_VERSION_MINOR = ((__version_number__ & 0x00ff00) >> 8)
LPY_VERSION_REV   = (__version_number__ & 0x0000ff)

LPY_VERSION_STR = str(LPY_VERSION_MAJOR)+'.'+str(LPY_VERSION_MINOR)+'.'+str(LPY_VERSION_REV)
if len(__revision__) > 0:
    LPY_VERSION_STR +='-r'+__revision__
