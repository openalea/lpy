from openalea.plantgl.all import *

def import_contours(fn):
    f = open(fn,'r')
    # read header of the file
    f.readline()
    nbitems = int(f.readline().split()[1])
    result = []
    for i in xrange(nbitems):
       f.readline()
       name = f.readline().split()[1]
       nbpoints = int(f.readline().split()[1])
       typecurve = f.readline().split()[1]
       samples = None
       if typecurve == 'or':
         samples = int(f.readline().split()[1])
       points = []
       for j in xrange(nbpoints):
           coord = map(float,f.readline().split())
           point = Vector4(coord[:3]+[1])
           for k in xrange(int(coord[3])):
               points.append(point) 
       if sum([p.z for p in points]) > 0:
            n = NurbsCurve(points)
       else:
            n = NurbsCurve2D([(p.x,p.y,p.w) for p in points])        
       if samples != None:
            n.strides = samples
       n.name = name
       result.append(n)
    return result