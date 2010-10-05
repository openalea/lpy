__version_number__ = 0x010B00
__revision_str__="$Revision$"
__revision__ = __revision_str__.split(':')[1][1:-1]

LPY_VERSION_MAJOR = ((__version_number__ & 0xff0000) >> 16)
LPY_VERSION_MINOR = ((__version_number__ & 0x00ff00) >> 8)
LPY_VERSION_REV   = (__version_number__ & 0x0000ff)

LPY_NUM_VERSION_STR = str(LPY_VERSION_MAJOR)+'.'+str(LPY_VERSION_MINOR)+'.'+str(LPY_VERSION_REV)
LPY_RELEASE_STR = str(LPY_VERSION_MAJOR)+'.'+str(LPY_VERSION_MINOR)

if len(__revision__) > 0:
    LPY_VERSION_STR = LPY_NUM_VERSION_STR+'-r'+__revision__
else:
    LPY_VERSION_STR = LPY_NUM_VERSION_STR
