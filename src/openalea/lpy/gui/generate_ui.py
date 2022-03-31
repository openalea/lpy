# Generate GUI if necessary
from os.path import dirname, exists, join
ldir = dirname(__file__)
release = exists(join(ldir,'py2exe_release.py'))

if not release:
    try:
        import compile_ui as ui
    except ImportError as ie:
        from . import compile_ui as ui
    ui.check_rc_generation(join(ldir, 'lpyresources.qrc'))
    ui.check_ui_generation(join(ldir, 'lpymainwindow.ui'))
    ui.check_ui_generation(join(ldir, 'debugger_ui.ui'))
    ui.check_ui_generation(join(ldir, 'debugger_right_ui.ui'))
    ui.check_ui_generation(join(ldir, 'logindialog.ui'))
    ui.check_ui_generation(join(ldir, 'logdialog.ui'))
    ui.check_ui_generation(join(ldir, 'killsimulationwidget.ui'))
    ui.check_ui_generation(join(ldir, 'lpyprefwidget.ui'))    
    ui.check_ui_generation(join(ldir, 'scalarmetaedit.ui'))
    ui.check_ui_generation(join(ldir, 'scalarfloatmetaedit.ui'))
    del ldir
    pass
