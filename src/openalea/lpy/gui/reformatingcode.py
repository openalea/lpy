"""
QObject.connect\(([a-zA-Z.]+),[ ]*SIGNAL\('(.*)\(.*\)'\)*,*(.+)\)
qt\.QtCore\.QObject\.connect\(([a-zA-Z.]+),[ ]*qt\.QtCore\.SIGNAL\(['"](.*)\(.*\)['"]\)*,*(.+)\)
\1.\2.connect(\3)
"""

def reformat(filename):
    filetext = file(filename,'r').read()
    qt_classmap = generate_qt_classmap(filetext)
    importheader = detect_end_imports(filetext)
    newfiletext = filetext[:importheader]+'\n'
    newfiletext += generate_qt_header(qt_classmap)+'\n'
    simpfiletext = simmlify_code(filetext[importheader:], qt_classmap)
    newfiletext += detect_signals(simpfiletext)
    nfile = file(filename+'.rfmat','w')
    nfile.write(newfiletext)
    return newfiletext


def detect_end_imports(filetext):
    lines = filetext.splitlines(True)
    res = 0
    for i, oline in enumerate(lines):
        line = oline.strip()
        if line.startswith('import') or line.startswith('from') or line.startswith('#') or line.startswith('try') or line.startswith('except') or len(line) == 0:
            res += len(oline)
            continue
        break
    return res


def detect_signals(filetext):
    import re  # (?<sigparam>[a-zA-Z_, \t]*)
    patternEmit = '(?P<sender>[a-zA-Z_\.]*).emit\\(SIGNAL\\([\'"](?P<signalname>[a-zA-Z_]+)\\((?P<sigparam>.*)\\)[\'"]\\),?(?P<param>.*)\\)'
    patternConnect = 'QObject\.connect\((?P<sender>.+),[ ]*SIGNAL\([\'"](?P<signalname>.+)\((?P<sigparam>.*)\)[\'"]\)[ ]*,[ ]*(?P<slot>.+)\)'
    patternDisonnect = patternConnect.replace('connect','disconnect')
    cclass, cline = None, None
    signals = []
    res = ''
    iline = 0
    for oline in filetext.splitlines(True):
        if oline.startswith('class'):
            print oline, oline.split(' \t')
            cclass = re.split('[ \t:(]',oline)[1]
            cline = iline 
            res += oline
        elif 'self.emit' in oline:
            m = re.search(patternEmit, oline)
            if m:
                gd = m.groupdict()
                res += oline[:m.regs[0][0]]
                sigparam = gd['sigparam']
                if sigparam == 'PyQt_PyObject': sigparam = "'PyQt_PyObject'"
                res += "#" + gd['sender']+'.'+gd['signalname']+' = pyqtSignal('+sigparam+') # AUTO SIGNAL TRANSLATION in class '+ cclass +'\n'
                res += oline[:m.regs[0][0]]
                res +=gd['sender']+'.'+gd['signalname']+'.emit('+gd['param']+')'+ oline[m.regs[0][1]:-1].rstrip() + " # " + oline[m.regs[0][0]:m.regs[0][1]] +" # AUTO SIGNAL TRANSLATION\n"
            else:
                res += oline
        elif 'disconnect' in oline:
            m = re.search(patternConnect, oline)
            if m:
                gd = m.groupdict()
                res += oline[:m.regs[0][0]]
                res += oline[:m.regs[0][0]]
                res += gd['sender']+'.'+gd['signalname']+'.disconnect('+gd['slot']+')'+ oline[m.regs[0][1]:].rstrip() + " # " + oline[m.regs[0][0]:m.regs[0][1]] + '\n'
            else:
                res += oline
        elif 'connect' in oline:
            m = re.search(patternConnect, oline)
            if m:
                gd = m.groupdict()
                res += oline[:m.regs[0][0]]
                sigparam = gd['sigparam']
                if sigparam == 'PyQt_PyObject': sigparam = "'PyQt_PyObject'"
                if gd['sender'] != 'self' : res += "# "
                res += gd['sender']+'.'+gd['signalname']+' = pyqtSignal('+sigparam+') # AUTO SIGNAL TRANSLATION in class '+ cclass +'\n'
                res += oline[:m.regs[0][0]]
                res += gd['sender']+'.'+gd['signalname']+'.connect('+gd['slot']+')'+ oline[m.regs[0][1]:].rstrip() + " # " + oline[m.regs[0][0]:m.regs[0][1]] + '\n'
            else:
                res += oline
        else:
           res += oline
        iline += len(oline)
    return res

def generate_qt_classmap(filetext):
    start = 0
    nextqtcmd = filetext.find('qt.', start)
    found = {}
    while nextqtcmd != -1:
        begpattern = filetext.find('.',nextqtcmd+3)+1
        endpattern = begpattern
        while filetext[endpattern].isalnum() or filetext[endpattern] == '_':
            endpattern += 1
        found[filetext[begpattern:endpattern]] = filetext[nextqtcmd:endpattern]
        nextqtcmd = endpattern
        nextqtcmd = filetext.find('qt.', nextqtcmd)
    return found

def generate_qt_header(qt_classmap):
    import PyQt5.QtWidgets as qwmodule
    import PyQt5.QtPrintSupport as qpmodule
    qw = set(qwmodule.__dict__.keys())
    qp = set(qpmodule.__dict__.keys())
    classmap = []
    for key, value in qt_classmap.items():
        nvalue = value.split('.')
        if key == 'SIGNAL':
            key = 'pyqtSignal'
            nvalue[2] = key
        if nvalue[1] == 'QtGui':
            if nvalue[2] in qw:
                nvalue[1] = 'QtWidgets'
            elif nvalue[2] in qp:
                nvalue[1] = 'QtPrintSupport'
        classmap.append((key, '.'.join(nvalue)))
    classmap.sort(cmp = lambda a,b : cmp(a[1],b[1]))
    res = ''
    for key, value in classmap:
        res += key+' = '+value+'\n'
    return res

def simmlify_code(filetext,qt_classmap):
    nfiletext = filetext
    for nclass, oclass in qt_classmap.items():
        nfiletext = nfiletext.replace(oclass, nclass)
    return nfiletext

print reformat("settings.py")