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
            n.stride = samples
       n.name = name
       result.append(n)
    return result

def import_functions(fn):
    f = open(fn,'r')
    # read header of the file
    f.readline() # funcgalleryver 1 1
    nbitems = int(f.readline().split()[1]) # items: 10
    result = []
    for i in xrange(nbitems):
       f.readline() # fver 1 1
       name = f.readline().split()[1] # name: BR_LEN_0
       samples = int(f.readline().split()[1]) # samples: 5
       f.readline() # flip: off
       nbpoints = int(f.readline().split()[1]) # points: 4
       points = []
       for j in xrange(nbpoints):
           coord = map(float,f.readline().split()) # 0.000000 0.577929
           point = Vector3(coord+[1])
           points.append(point) 
       n = NurbsCurve2D(points)        
       if samples != None:
            n.stride = samples
       n.name = name
       result.append(n)
    return result

def import_function(fn):
    import os.path
    f = open(fn,'r')
    # read header of the file
    f.readline() # range 0, 1
    nbpoints = int(f.readline().split()[1]) # points: 4
    points = []
    for j in xrange(nbpoints):
       coord = map(float,f.readline().split()) # 0.000000 0.577929
       point = Vector3(coord+[1])
       points.append(point) 
    result = NurbsCurve2D(points)
    name = os.path.basename(os.path.splitext(fn)[0])
    result.name = name
    return result

linetofloat = lambda l : map(float,l.split())
def vec3inline(l,i,j,k):
    values = l.split()
    return Vector3(float(values[i]),float(values[j]),float(values[k]))

def import_patch(fn):
    from os.path import basename,splitext
    f = open(fn,'r')
    base = splitext(basename(fn))[0]
    # read header of the file
    f.next() # bbox
    nextline = f.next()
    # PRECISION S: s T: t
    if nextline.split()[0] == 'PRECISION':
        nextline = f.next()
    # CONTACT POINT X: x Y: y Z: z
    contact = vec3inline(nextline,3,5,7)
    # END POINT X: x Y: y Z: z
    f.next()
    # HEADING X: x Y: y Z: z
    heading = vec3inline(f.next(),2,4,6)
    l = heading.normalize()
    # UP X: x Y: y Z: z
    up = vec3inline(f.next(),2,4,6)
    up.normalize()
    # SIZE: x
    size = float(f.next().split()[1])
    patchlist = []
    while True :
        try :
            nextline = f.next()
        except:
            break
        nextline = nextline.strip()
        dobreak = False
        while len(nextline) == 0 :
            try :
                nextline = f.next()
                nextline = nextline.strip()
            except:
                dobreak = True
                break
        if dobreak:
            break
        # patchname
        name = nextline
        # TOP COLOR: i DIFFUSE: x BOTTOM COLOR: i DIFFUSE: y
        # AL: patch1 A: patch2 AR: patch3
        # L: patch4 R: patch5
        # BL: patch6 B: patch7 BR: patch8
        for i in xrange(4): f.next()
        ctrlpoints = []
        left = heading^up
        m = Matrix3(-up,-left,heading)
        #m = Matrix3(-left, -heading, up)
        m = m.inverse()
        for i in xrange(4):
            v = f.next().split()
            row = []
            for j in range(4):
                p = Vector3(float(v[j*3]),float(v[j*3+1]),float(v[j*3+2]))
                p -= contact
                p = m*p
                row.append(Vector4(p,1))
            ctrlpoints.append(row)    
        smb = BezierPatch(ctrlpoints)
        actualdim = 2*max(BoundingBox(smb).getSize())
        smb = BezierPatch([[Vector4(i.project()/actualdim,1) for i in ctrllines] for ctrllines in ctrlpoints],ustride = 10, vstride = 10)
        smb.name = base+'_'+name
        patchlist.append(smb)
    return patchlist

def import_colormap(fn):
    import array
    a = array.array('B')
    a.fromfile(open(fn,'rb'),256*3)
    return [Material('Color_'+str(i),Color3(a[3*i],a[3*i+1],a[3*i+2]),diffuse=0) for i in xrange(256)]

def import_materialmap(fn):
    import array
    stream = open(fn,'rb')
    result = []
    while True:
        a = array.array('B')
        try:
            a.fromfile(stream,15)
        except Exception,e:
            break
        valiter = iter(a)
        id = valiter.next()
        transparency = valiter.next()
        ambient = (valiter.next(),valiter.next(),valiter.next())
        diffuse = (valiter.next(),valiter.next(),valiter.next())
        emission = Color3(valiter.next(),valiter.next(),valiter.next())
        specular = Color3(valiter.next(),valiter.next(),valiter.next())
        shininess = valiter.next()
        sdiffuse = sum(diffuse)
        sambient = sum(ambient)
        if sdiffuse > 0 and sambient > 0:
            ambient_ratio = sambient/float(sdiffuse)
            m = Material('Color_'+str(id),Color3(*(int(i * ambient_ratio) for i in diffuse)),1./ambient_ratio,specular,emission,shininess,transparency)
        else:
            m = Material('Color_'+str(id),Color3(*ambient),0,specular,emission,shininess,transparency)
            
        result.append(m)
    return result