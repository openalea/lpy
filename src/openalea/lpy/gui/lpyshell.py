

from qtconsole.rich_jupyter_widget import RichJupyterWidget
from qtconsole.inprocess import QtInProcessKernelManager
from .streamredirection import GraphicalStreamRedirection

class LpyShellWidget(RichJupyterWidget, GraphicalStreamRedirection):

    def __init__(self, parent=None):
        """
        :param  parent: specifies the parent widget.
        If no parent widget has been specified, it is possible to
        exit the interpreter by Ctrl-D.
        """

        RichJupyterWidget.__init__(self, parent)

        self.kernel_manager = QtInProcessKernelManager()
        self.kernel_manager.start_kernel(show_banner=False)

        self.kernel = self.kernel_manager.kernel
        self.kernel.gui = 'qt4'
        self.shell = self.kernel.shell


        self.kernel_client = self.kernel_manager.client()
        self.kernel_client.start_channels()

        self.kernel.locals = self.kernel.shell.user_ns

        # Multiple Stream Redirection
        GraphicalStreamRedirection.__init__(self, self.kernel.stdout, self.kernel.stderr)


def set_shell_widget(lpywidget):
    #import sip
    #assert sip.getapi('QString') == 2

    import sys

    ipython_widget = LpyShellWidget(lpywidget.interpreterDock)
    lpywidget.interpreterDock.setWidget(ipython_widget)

    kernel = ipython_widget.kernel

    lpywidget.shellwidget = ipython_widget
    lpywidget.interpreter = kernel
    lpywidget.shell = kernel.shell

    lpywidget.interpreter.locals['window'] = lpywidget
    lpywidget.shell.run_code('from openalea.plantgl.all import *')
    lpywidget.shell.run_code('from openalea.lpy import *')
