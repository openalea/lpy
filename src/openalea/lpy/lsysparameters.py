from openalea.plantgl.all import PglTurtle, PyStrPrinter, Material, NurbsCurve2D, BezierCurve2D, Polyline2D, NurbsPatch
import openalea.plantgl.all as pgl
from .__lpy_kernel__ import LpyParsing, LsysContext

default_credits = {'__authors__'    : '' ,
                   '__institutes__'  : '' ,
                   '__copyright__'   : '' ,
                   '__description__' : '' ,
                   '__references__'  : '' }

default_lpycode_version = 1.2
default_lpyjson_version = 1.0

scalartypemap = { int : 'Integer', float :'Float', bool : 'Bool'}
graphictypemap = { NurbsCurve2D : 'Curve2D', BezierCurve2D : 'Curve2D', Polyline2D : 'Curve2D', NurbsPatch : 'NurbsPatch'}

defaultturtlecolorlist = PglTurtle().getColorList()

def isSimilarToDefaultTurtleMat(cmat, i):
    if cmat.isTexture() : return False
    
    nbdefault = len(defaultturtlecolorlist)

    if i >= nbdefault: 
        defaultmat = Material('Color_'+str(i))
    else:
        defaultmat = Material(defaultturtlecolorlist[i])
    if cmat.isSimilar(defaultmat) and cmat.name == defaultmat.name:
        return True
    else:
        return False


class LsystemParameters:
    def __init__(self, lsystem = None):
        self.execOptions = {}
        self.animation_timestep = None

        self.colorList = []

        self.scalars = []
        self.graphicalparameters = [({'name':'default'},[])]
        self.credits = default_credits

        if lsystem:
            self.retrieve_from(lsystem)

    def check_validity(self):
        from openalea.lpy.gui.abstractobjectmanager import AbstractObjectManager
        from openalea.lpy.parameters.scalar import BaseScalar
        assert isinstance(self.execOptions, dict)
        assert isinstance(self.credits, dict)
        if not self.animation_timestep is None:
            assert self.animation_timestep > 0
        for value in self.colorList:
            assert isinstance(value, pgl.Appearance)
        for value in self.scalars:
            assert isinstance(value, BaseScalar)
        for pinfo, pvalues in self.graphicalparameters:
            assert isinstance(pinfo, dict)
            assert 'name' in pinfo
            for pmanager, pvalue in pvalues:
                assert isinstance(pmanager, AbstractObjectManager)
                assert isinstance(pvalue, pgl.SceneObject) # ??

    def is_valid(self):
        try:
            self.check_validity()
            return True
        except:
            return False

    def is_similar(self, other):
        try:
            self.check_similarity()
            return True
        except:
            return False

    def check_similarity(self, other):
        from itertools import zip_longest
        import openalea.plantgl.scenegraph.pglinspect as inspect 

        if self.execOptions != other.execOptions: 
            raise ValueError('execOptions',self.execOptions,other.execOptions)
        if self.credits != other.credits: 
            raise ValueError('credits',self.credits,other.credits)
        if self.animation_timestep != other.animation_timestep: 
            raise ValueError('animation_timestep',self.animation_timestep,other.animation_timestep)
        for i,(v1,v2) in enumerate(zip_longest(self.colorList, other.colorList)):
            if v1 is None and not v2 is None :
                if not isSimilarToDefaultTurtleMat(v2,i):
                    raise ValueError(v1,v2)
            elif v2 is None and not v1 is None :
                if not isSimilarToDefaultTurtleMat(v1,i):
                    raise ValueError(v1,v2)
            elif not  v1.isSimilar(v2): 
                raise ValueError(v1,v2)
        for v1,v2 in zip_longest(self.scalars, other.scalars):
            if v1.scalartype() != v2.scalartype() or v1.value != v2.value:
                raise ValueError('scalars',v1,v2)

        def similar_pgl_object(v1,v2):
            attributes = inspect.get_pgl_attributes(v1)            
            for att in attributes:
                if getattr(v1,att) != getattr(v2,att):
                    if not isinstance(getattr(v1,att),pgl.PglObject): # cannot compare easily
                        raise ValueError(att,getattr(v1,att),getattr(v2,att))
            return True

        for (i1,v1),(i2,v2) in zip_longest(self.graphicalparameters, other.graphicalparameters):
            if i1 != i2 :
                raise ValueError('category',i1,i2)
            for (m1, v11),(m2,v22) in zip_longest(v1,v2):
                if m1 != m2 :
                    raise ValueError('manager',m1.name,m2.name)
                if type(v11) != type(v22) :
                    raise ValueError('type',v11,v22)
                if not similar_pgl_object(v11,v22) :
                    raise ValueError('object',v11,v22)

    def available_parameter_types(self):
        return self.available_scalar_types()+self.available_graphical_types()

    def available_scalar_types(self):
        return scalartypemap.values()

    def get_graphicalparameter_managers(self):
        from openalea.lpy.gui.objectmanagers import get_managers
        return get_managers()

    def available_graphical_types(self):
        return list(self.get_graphicalparameter_managers().keys())

    def add(self, name, value, ptype = None, category = None, **params):
        if ptype is None:
            ptype = type(value)
            ptype = scalartypemap.get(ptype,graphictypemap.get(ptype,ptype))
        if ptype in self.available_scalar_types():
            self.add_scalar(name, value, ptype, category, **params)
        elif ptype in self.available_graphical_types():
            self.add_graphicalparameter(name, value, ptype, category, **params)
        else:
            raise TypeError(ptype)

    def add_scalar(self, name, value, ptype = None, category = None, **params):
        from .parameters.scalar import ProduceScalar
        if ptype is None:
            ptype = scalartypemap[type(value)]
        if category is None:
            category = 'default'
        assert ptype in self.available_scalar_types()
        scalar = ProduceScalar((name, ptype, value))
        scalar.__dict__.update(**params)
        scalar.category = category
        if len(self.scalars) > 2:
            found = False
            for i,(sc,prevc) in enumerate(zip(self.scalars[1:],self.scalars[:-1])):
                if prevc.category == category and sc.category != category:
                    self.scalars.insert(i+1,scalar)
                    found = True
                    break
            if not found:
                self.scalars.append(scalar)
        else:
                self.scalars.append(scalar)

    def add_function(self, name, value, category = None):
        self.add_graphicalparameter(name, value, 'Function', category)

    def add_graphicalparameter(self, name, value, ptype = None, category = None):
        assert ptype in self.available_graphical_types()
        if ptype is None:
            ptype = graphictypemap[type(value)]
        if category is None:
            category = 'default'
        selectedset = []
        for pinfo, pset in self.graphicalparameters:
            if pinfo['name'] == category:
                selectedset = pset
                break
        else:
            self.graphicalparameters.append(({'name' : category, 'enabled' : True }, selectedset))
        manager = self.get_graphicalparameter_managers()[ptype]
        manager.setName(value, name)
        selectedset.append((manager, value))

    def set_options(self, name, value):
        self.execOptions[name] = value

    def set_color(self, index, value):
        self.colorList[index] = value

    def categories(self):
        return list(set([pinfo['name'] for pinfo, pset in self.graphicalparameters]+[sc.category for sc in self.scalars]))

    def category_info(self, name):
        for pinfo, pset in self.graphicalparameters:
            if pinfo['name']== name:
                return pinfo
        for sc in scalars:
            if sc.category == name:
                return { 'name' : name, 'enabled' : True}

    def category_parameters(self, name):
        result = []
        for pinfo, pset in self.graphicalparameters:
            if pinfo['name']== name:
                result += pset
                break
        for sc in scalars:
            if sc.category == name:
                result.append(sc)
        return result

    def retrieve_from(self, lsystem):
        #self.lsystem = lsystem
        #self.reference_dir = reference_dir
        self.retrieve_from_env(lsystem.execContext())

    def retrieve_from_env(self, context):
        self._retrieve_code_version_from_env(context)
        self._retrieve_exec_parameters_from_env(context)
        self._retrieve_colors_from_env(context)
        self._retrieve_credits_from_env(context)
        self._retrieve_scalars_from_env(context)
        self._retrieve_graphical_parameters_from_env(context)

    def _retrieve_code_version_from_env(self, context):
        self.code_version = context.get('__lpy_code_version__',default_lpycode_version)

    def _retrieve_credits_from_env(self, context):
        credits = {}
        for  c in default_credits.keys():
            credits[c] = context.get(c,'')

    def _retrieve_scalars_from_env(self, context):
        from .parameters.scalar import ProduceScalar
        scalars = context.get('__scalars__', [])
        self.scalars = []
        currentcategory = ''
        for sc in scalars:
            if sc[1] is None: currentcategory = sc[0]
            csc = ProduceScalar(v)
            csc.category = currentcategory

    def _retrieve_graphical_parameters_from_env(self, context):
        managers = self.get_graphicalparameter_managers()

        def checkinfo(info): return {'name':info} if type(info) == str else info
        self.graphicalparameters = context.get('__parameterset__',[])
        self.graphicalparameters = [ (checkinfo(panelinfo), [(managers[typename],obj) for typename,obj in objects]) for panelinfo,objects in self.graphicalparameters]

    def _retrieve_colors_from_env(self, context):
        self.colorList = context.turtle.getColorList()

    def _retrieve_exec_parameters_from_env(self, context):
        self.animation_timestep = None
        if not context.is_animation_timestep_to_default():
            self.animation_timestep = context.animation_timestep
        self.execOptions = {}
        for option in context.options:
            if not option.isToDefault():
                self.execOptions[option.name] = option.selection

    def generate_py_code(self, indentation = '', reference_dir = None):
        code = self._generate_main_py_code(indentation, reference_dir)
        code += self._generate_credits_py_code(indentation)
        if len(code) > 0:
            code = LpyParsing.InitialisationBeginTag+'\n\n'+'__lpy_code_version__ = '+str(default_lpycode_version)+'\n\n'+code
        return code

    def _generate_main_py_code(self, indentation = '', reference_dir = None):
        header = indentation+"def "+LsysContext.InitialisationFunctionName+"(context):\n"
        init_txt = self._generate_exec_parameters_py_code(indentation+'\t')
        init_txt += self._generate_colors_py_code(indentation+'\t', reference_dir)
        init_txt += self._generate_graphical_parameters_py_code(indentation+'\t')
        init_txt += self._generate_scalars_py_code(indentation+'\t')
        if len(init_txt) > 0:
            return header+init_txt
        else:
            return ''

    def _generate_colors_py_code(self, indentation = '\t', reference_dir = None):
        from openalea.plantgl.all import Material, PglTurtle, PyStrPrinter
        if self.colorList is None: return ''

        init_txt = ''
        nbcurrent = len(self.colorList)

        printer = PyStrPrinter()
        printer.pglnamespace = 'pgl'
        printer.indentation = indentation
        printer.indentation_increment = '\t'
        printer.line_between_object = 0
        if reference_dir and len(reference_dir) > 0:
            printer.reference_dir = os.path.abspath(reference_dir)

        firstcol = True
        for i, cmat in enumerate(self.colorList):
            if not isSimilarToDefaultTurtleMat(cmat, i):
                if firstcol :
                    init_txt += indentation+"import openalea.plantgl.all as pgl\n"
                    firstcol = False
                if cmat.name == '':
                    cmat.name = 'Color_'+str(i)
                cmat.apply(printer)
                init_txt += printer.str()
                printer.clear()
                init_txt += indentation+'context.turtle.setMaterial('+repr(i)+','+str(cmat.name)+')\n'
        return init_txt      

    def _generate_exec_parameters_py_code(self, indentation = '\t'):
        init_txt = ''
        if not self.animation_timestep is None:
            init_txt += indentation+'context.animation_timestep = '+str(self.animation_timestep)+'\n'           
        for optname, optvalue in self.execOptions.items():
            init_txt += indentation+'context.options.setSelection('+repr(options[i].name)+','+str(options[i].selection)+')\n'
        return init_txt

    def _generate_graphical_parameters_py_code(self, indentation = '\t'):
        init_txt = ''
        if self.graphicalparameters is None: return init_txt
        def emptyparameterset(params):
            for panel,data in params:
                if len(data) > 0: return False
            return True
        if not emptyparameterset(self.graphicalparameters) :
            intialized_managers = {}
            panelid = 0
            for panelinfo,objects in self.graphicalparameters:
                if panelinfo.get('active',True) or withall:
                    for manager,obj in objects:
                        if manager not in intialized_managers:
                            intialized_managers[manager] = True
                            init_txt += manager.initWriting('\t') 
                        init_txt += manager.writeObject(obj,'\t')
                    init_txt += indentation+'panel_'+str(panelid)+' = ('+repr(panelinfo)+',['+','.join(['('+repr(manager.typename)+','+manager.getName(obj)+')' for manager,obj in objects])+'])\n'
                panelid += 1    
            init_txt += indentation+'parameterset = ['
            panelid = 0
            for panelinfo,objects in self.graphicalparameters:
                if panelinfo.get('active',True) or withall:
                    init_txt += 'panel_'+str(panelid)+','
                panelid += 1
            init_txt += ']\n'
            init_txt += indentation+'context["__parameterset__"] = parameterset\n'
            for panelinfo,objects in self.graphicalparameters:
                if panelinfo.get('active',True):
                    for manager,obj in objects:
                        init_txt += indentation+'context["'+manager.getName(obj)+'"] = '+manager.writeObjectToLsysContext(obj) + '\n'
        return init_txt

    def _generate_scalars_py_code(self, indentation = '\t'):
        init_txt = ''
        if not self.scalars is None and len(self.scalars) > 0:
            init_txt += indentation+'scalars = '+str([sc.tostr() for sc in self.scalars])+'\n'
            init_txt += indentation+'context["__scalars__"] = scalars\n'
            init_txt += indentation+'for s in scalars:\n'+indentation+'\tif not s[1] == "Category" : context[s[0]] = s[2]\n'
        return init_txt

    def _generate_credits_py_code(self, indentation = '\t'):
        txt = ''
        for key,value in self.credits.items():             
            if len(value) > 0:
                txt += indentation+key+' = '+repr(str(value))+'\n'
        return txt

    def generate_json_parameter_dict(self):
        import openalea.plantgl.algo.jsonrep as jrep
        from collections import OrderedDict 
        parameters = OrderedDict()

        for panelinfo, objects in self.graphicalparameters:
            panel = panelinfo.copy()
            panel.setdefault('enabled',True)
            panel['scalars'] = []
            items = []
            for manager,obj in objects:
                items.append(manager.to_json(obj))
            panel['items'] = items
            parameters[panel['name']] = panel

        for sc in self.scalars:
            if not sc.category in parameters:
                parameters[sc.category] = dict(name=sc.category, enabled=True, items=[], scalars=[])
            parameters[sc.category]['scalars'].append(sc.todict())

        defaultlist = PglTurtle().getColorList()
        materials = []
        for i, cmat in enumerate(self.colorList):
            if not isSimilarToDefaultTurtleMat(cmat, i):
                materials.append(jrep.to_json_rep(m,i))

        if self.animation_timestep is None:
            options = self.execOptions
        else:
            options = self.execOptions.copy()
            options['animation_timestep'] = self.animation_timestep

        result =  dict(
            schema = 'lpy',
            version = str(default_lpyjson_version),
            options = options,
            materials = materials,
            parameters = list(parameters.values()),
            credits = dict([(key,value) for key,value in self.credits.items() if value != ''])
        )
        assert LsystemParameters.validate_schema(result)
        return result     

    @staticmethod
    def validate_schema(obj):
        # TODO: load files only once
        import io, os, json, jsonschema
        is_valid = False
        schema_path = os.path.join(os.path.dirname(__file__), 'parameters', 'schema')
        with io.open(os.path.join(schema_path, 'lpy.json'), 'r') as schema_file:
            try:
                schema = json.loads(schema_file.read())
            except json.JSONDecodeError as e:
                print(e)
            resolver = jsonschema.RefResolver(f'file:///{schema_path}/', schema)
            try:
                jsonschema.validate(obj, schema, format_checker=jsonschema.draft7_format_checker, resolver=resolver)
                is_valid = True
            except jsonschema.exceptions.ValidationError as e:
                print('L-Py schema validation failed:', e.message)
            except jsonschema.exceptions.RefResolutionError as e:
                print('JSON schema $ref file not found:', e)
        return is_valid

    def retrieve_from_json_dict(self, obj):
        import openalea.plantgl.algo.jsonrep as jrep
        from openalea.lpy.parameters.scalar import scalar_from_json_rep
        assert LsystemParameters.validate_schema(obj)
        self.credits.update(obj['credits'])
        options = obj['options'].copy()
        if 'animation_timestep' in options:
            self.animation_timestep = options['animation_timestep']
            del options['animation_timestep']
        self.execOptions = options
        materials = jrep.from_json_rep(obj['materials'])
        parameters = obj['parameters']
        scalars = []
        gparameters = []
        managers = self.get_graphicalparameter_managers()
        for pset in parameters:
            lscalars = list(map(scalar_from_json_rep,pset['scalars']))
            for sc in lscalars:
                sc.category = pset['name']
            scalars += lscalars
            lpset = []
            pinfo = {}
            for propname in pset.keys():
                if not propname in ['items','scalars']:
                    pinfo[propname] = pset[propname]
            for v in pset['items']:
                managername = None
                if 'is_function' in v:
                    if v['is_function']:
                        managername = 'Function'
                    del v['is_function']
                obj = jrep.from_json_rep(v)
                if managername is None:
                    managername = graphictypemap[type(obj)]
                manager = managers[managername]
                lpset.append((manager,obj))
            gparameters.append((pinfo,lpset))

        self.graphicalparameters = gparameters
        self.scalars = scalars
        # TOFINISH
