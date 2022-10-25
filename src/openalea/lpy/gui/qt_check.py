from openalea.plantgl.all import get_pgl_qt_version
import os

QT_VERSION = get_pgl_qt_version() >> 16

if QT_VERSION == 6:
    os.environ['QT_API'] = 'pyside6'
elif QT_VERSION == 4:
    os.environ['QT_API'] = 'pyqt'
else:
    os.environ['QT_API'] ='pyqt'+str(QT_VERSION)


if QT_VERSION == 4:
    os.environ.setdefault('QT_API_VERSION', '2')

from openalea.plantgl.gui import qt

if QT_VERSION == 4:
    import sip
    assert sip.getapi('QString') == 2
