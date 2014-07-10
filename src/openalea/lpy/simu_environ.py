from openalea.plantgl.all import PglTurtle, PyStrPrinter, Material
from __lpy_kernel__ import LpyParsing, LsysContext

def getInitialisationCode(context = None, scalars = None, visualparameters = None, credits = None, colorlist = None, simplified = False,
                           keepCode_1_0_Compatibility = False, referencedir = None):
    code = initialisationFunction(context,scalars,visualparameters, simplified)
    if credits : code += creditsCode(credits)
    if len(code) > 0:
        code = LpyParsing.InitialisationBeginTag+'\n\n'+'__lpy_code_version__ = '+str(1.1)+'\n\n'+code
    return code

def initialisationFunction(context = None, 
                           scalars = None, 
                           visualparameters = None,
                           colorlist = None, 
                           simplified = False,
                           keepCode_1_0_Compatibility = False, 
                           referencedir = None):
    header = "def "+LsysContext.InitialisationFunctionName+"(context):\n"
    init_txt = ''
    if not simplified:
        if not colorlist and context: colorlist = context.turtle.getColorList()
        if colorlist: init_txt += colorListCode(colorlist,referencedir)
        if context: init_txt += contextOptionCode(context)    
    if scalars: init_txt += scalarCode(scalars)
    if visualparameters: init_txt += visualParametersCode(visualparameters,keepCode_1_0_Compatibility,simplified)
    # we return code only if necessary
    if len(init_txt) > 0:  return header+init_txt
    else:                  return '' 
    
def visualParametersCode(visualparameters = None, 
                        keepCode_1_0_Compatibility = False,
                        simplified = False):
    init_txt = ''
    def emptyparameterset(params):
        for panel,data in params:
            if len(data) > 0: return False
        return True
    if not visualparameters is None and not emptyparameterset(visualparameters) :
        intialized_managers = {}
        panelid = 0
        for panelinfo,objects in visualparameters:
            if panelinfo.get('active',True) or not simplified:
                for manager,obj in objects:
                    if not intialized_managers.has_key(manager):
                        intialized_managers[manager] = True
                        init_txt += manager.initWriting('\t')
                    init_txt += manager.writeObject(obj,'\t')
                init_txt += '\tpanel_'+str(panelid)+' = ('+repr(panelinfo)+',['+','.join(['('+repr(manager.typename)+','+manager.getName(obj)+')' for manager,obj in objects])+'])\n'
            panelid += 1    
        init_txt += '\tparameterset = ['
        panelid = 0
        for panelinfo,objects in visualparameters:
            if panelinfo.get('active',True) or not simplified:
                init_txt += 'panel_'+str(panelid)+','
            panelid += 1
        init_txt += ']\n'
        if not simplified and keepCode_1_0_Compatibility:
            init_txt += '\tcontext["__functions__"] = ['
            for panelinfo,objects in visualparameters:
                if panelinfo.get('active',True):
                    for manager,obj in objects:
                        if manager.typename == 'Function':
                            init_txt += '('+repr(manager.getName(obj))+','+manager.getName(obj)+'),'
            init_txt += ']\n'
            init_txt += '\tcontext["__curves__"] = ['
            for panelinfo,objects in visualparameters:
                if panelinfo.get('active',True):
                    for manager,obj in objects:
                        if manager.typename == 'Curve2D':
                            init_txt += '('+repr(manager.getName(obj))+','+manager.getName(obj)+'),'
            init_txt += ']\n'
            
        init_txt += '\tcontext["__parameterset__"] = parameterset\n'
        for panelinfo,objects in visualparameters:
            if panelinfo.get('active',True):
                for manager,obj in objects:
                    init_txt += '\tcontext["'+manager.getName(obj)+'"] = '+manager.writeObjectToLsysContext(obj) + '\n'
    return init_txt

def colorListCode(colorlist = None, referencedir = None, indentation = '\t'):
    init_txt = ''
    if not colorlist is None and len(colorlist) > 0:
        defaultlist = PglTurtle().getColorList()
        nbdefault = len(defaultlist)
        nbcurrent = len(colorlist)
        firstcol = True
        defaultmat = Material('default')
        printer = PyStrPrinter()
        printer.pglnamespace = 'pgl'
        printer.indentation = indentation
        printer.indentation_increment = '\t'
        printer.line_between_object = 0
        if referencedir:
            printer.reference_dir = referencedir
        for i in xrange(nbcurrent):
            cmat = colorlist[i]
            if ( (i >= nbdefault) or 
                (cmat.isTexture()) or
                (not cmat.isSimilar(defaultlist[i])) or 
                (cmat.name != defaultlist[i].name)):
                if cmat.isTexture() or not cmat.isSimilar(defaultmat):
                    if firstcol :
                        init_txt += indentation+"import openalea.plantgl.all as pgl\n"
                        firstcol = False
                    cmat.name = 'Color_'+str(i)
                    cmat.apply(printer)
                    init_txt += printer.str()
                    printer.clear()
                    init_txt += indentation+'context.turtle.setMaterial('+repr(i)+','+str(cmat.name)+')\n'
    return init_txt

def contextOptionCode(context,indentation = '\t'):
    init_txt = ''
    if not context is None:
        if not context.is_animation_timestep_to_default():
            init_txt += '\tcontext.animation_timestep = '+str(context.animation_timestep)+'\n'           
        options = context.options
        for i in xrange(len(options)):
            if not options[i].isToDefault():
                init_txt += '\tcontext.options.setSelection('+repr(options[i].name)+','+str(options[i].selection)+')\n'
    return init_txt

def scalarCode(scalars = None, indentation = '\t'):
    init_txt = ''
    if not scalars is None and len(scalars) > 0:
        init_txt += indentation+'scalars = '+str([str(i) for i in scalars])+'\n'
        init_txt += indentation+'context["__scalars__"] = scalars\n'
        init_txt += indentation+'for s in scalars:\n'+indentation+'\tif not s[1] == "Category" : context[s[0]] = s[2]\n'
    return init_txt
    
def creditsCode(desc_items):
    txt = ''
    for key,value in desc_items.iteritems():             
        if len(value) > 0:
            txt += key+' = '+repr(str(value))+'\n'
    return txt
