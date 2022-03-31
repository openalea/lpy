

from qtconsole.rich_jupyter_widget import RichJupyterWidget
from qtconsole.inprocess import QtInProcessKernelManager
# from .streamredirection import GraphicalStreamRedirection
import sys, tempfile

class LpyShellWidget(RichJupyterWidget): #, GraphicalStreamRedirection):

    def __init__(self, parent=None):
        """
        :param  parent: specifies the parent widget.
        If no parent widget has been specified, it is possible to
        exit the interpreter by Ctrl-D.
        """
        if sys.executable.endswith('pythonw.exe'):
            lpylog = open(tempfile.gettempdir() + '/lpylog.txt', 'w')
            sys.stdout = lpylog 
            sys.stderr = lpylog

        RichJupyterWidget.__init__(self, parent)

        self.kernel_manager = QtInProcessKernelManager()
        self.kernel_manager.start_kernel(show_banner=False)

        self.kernel = self.kernel_manager.kernel
        self.kernel.gui = 'qt'

        self.shell = self.kernel.shell

        self.kernel_client = self.kernel_manager.client()
        self.kernel_client.start_channels()

        self.kernel.locals = self.kernel.shell.user_ns

        # Multiple Stream Redirection
        # GraphicalStreamRedirection.__init__(self, self.kernel.stdout, self.kernel.stderr)


def set_shell_widget(lpywidget):
    ipython_widget = LpyShellWidget(lpywidget.interpreterDock)
    lpywidget.interpreterDock.setWidget(ipython_widget)

    kernel = ipython_widget.kernel

    lpywidget.shellwidget = ipython_widget
    lpywidget.interpreter = kernel
    lpywidget.shell = kernel.shell

    lpywidget.shellstdout = kernel.stdout
    lpywidget.shellstderr = kernel.stderr

    sys.stdout = kernel.stdout
    sys.stderr = kernel.stderr