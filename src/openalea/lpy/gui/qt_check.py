from openalea.plantgl.all import get_pgl_qt_version
import os

QT_VERSION = get_pgl_qt_version() >> 16
os.environ['QT_API'] = 'pyqt' if QT_VERSION == 4 else 'pyqt'+str(QT_VERSION)

from openalea.vpltk import qt
#from openalea.vpltk.qt.compat import *

if QT_VERSION == 4:
    os.environ.setdefault('QT_API_VERSION', '2')
if QT_VERSION == 4:
    import sip
    assert sip.getapi('QString') == 2
