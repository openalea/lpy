# Generate GUI if necessary
try:
    import openalea.lpy.gui.py2exe_release
    py2exe_release = True
except:
    py2exe_release = False

if not py2exe_release:
    import openalea.lpy.gui.compile_ui as ui
    import os.path
    ldir    = os.path.dirname(__file__)
    print("Generate Ui")
    ui.check_ui_generation(os.path.join(ldir, 'lpymainwindow.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'debugger_ui.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'debugger_right_ui.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'logindialog.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'logdialog.ui'))
    ui.check_rc_generation(os.path.join(ldir, 'lpyresources.qrc'))
    ui.check_ui_generation(os.path.join(ldir, 'killsimulationwidget.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'lpyprefwidget.ui'))    
    ui.check_ui_generation(os.path.join(ldir, 'scalarmetaedit.ui'))
    ui.check_ui_generation(os.path.join(ldir, 'scalarfloatmetaedit.ui'))
    del ldir
    pass
