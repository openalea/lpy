import openalea.lpy as lpy

class PlotRedirection:
    def __init__(self):
        class Plotter:
           def __init__(self): pass
           def plot(self,scene): pass
        self.myplotter = Plotter()
        lpy.registerPlotter(self.myplotter)
    def __del__(self):
        lpy.cleanPlotter()
           