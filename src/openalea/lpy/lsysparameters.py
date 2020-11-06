from openalea.plantgl.all import PglTurtle, PyStrPrinter, Material
from .__lpy_kernel__ import LpyParsing, LsysContext

default_credits = {'__authors__'    : '' ,
                   '__institutes__'  : '' ,
                   '__copyright__'   : '' ,
                   '__description__' : '' ,
                   '__references__'  : '' }

default_lpycode_version = 1.2

scalartypemap = { int : 'Integer', float :'Float', bool : 'Bool'}

class LsystemParameters:
    def __init__(self, lsystem = None, reference_dir = None):
        self.code_version = default_lpycode_version
        self.execOptions = {}
        self.animation_timestep = None

        self.colorList = None

        self.scalars = []
        self.graphicalparameters = [({'name':'default'},[])]
        self.credits = default_credits

        self.import_from(lsystem)

    def available_parameter_types(self):
        return self.available_scalar_types()+self.available_graphical_types()

    def available_scalar_types(self):
        return scalartypemap.values()

    def get_graphicalparameter_managers(self):
        from openalea.lpy.gui.objectmanagers import get_managers
        return get_managers()

    def available_graphical_types(self):
        return list(self.get_graphicalparameter_managers().keys())

    def add_parameter(self, name, value, ptype = None, category = None, **params):
        if ptype is None:
            ptype = scalartypemap[type(value)]
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

    def add_graphicalparameter(self, name, value, ptype = None, category = None):
        assert ptype in self.available_graphical_types()
        if category is None:
            category = 'default'
        selectedset = []
        for pinfo, pset in self.graphicalparameters:
            if pinfo['name'] == category:
                selectedset = pset
                break
        else:
            self.graphicalparameters.append(({'name' : category}, selectedset))
        manager = self.get_graphicalparameter_managers()[ptype]
        manager.setName(value, name)
        selectedset.append((manager, value))

    def import_from(self, lsystem):
        #self.lsystem = lsystem
        self.reference_dir = reference_dir
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
        from openalea.lpy.gui.objectmanagers import get_managers
        managers = get_managers()

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
            code = LpyParsing.InitialisationBeginTag+'\n\n'+'__lpy_code_version__ = '+str(1.1)+'\n\n'+code
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

        defaultlist = PglTurtle().getColorList()
        nbdefault = len(defaultlist)

        defaultmat = Material('default')

        printer = PyStrPrinter()
        printer.pglnamespace = 'pgl'
        printer.indentation = indentation
        printer.indentation_increment = '\t'
        printer.line_between_object = 0
        if reference_dir and len(reference_dir) > 0:
            printer.reference_dir = os.path.abspath(reference_dir)

        firstcol = True
        for i in range(nbcurrent):
            cmat = self.colorList[i]
            if ( (i >= nbdefault) or (cmat.isTexture()) or (not cmat.isSimilar(defaultlist[i])) or (cmat.name != defaultlist[i].name)):
                if cmat.isTexture() or not cmat.isSimilar(defaultmat):
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
        def col2list(col): return [col.red,col.green,col.blue]
        def mat2dict(mat,i):
            return dict(name=mat.name,
                        index=i,
                        ambient=col2list(mat.ambient),
                        diffuse=mat.diffuse,
                        specular=col2list(mat.specular),
                        emission=col2list(mat.emission),
                        transparency=mat.transparency,
                        shininess=mat.shininess)
        
        result =  dict(
            schema = 'lpy',
            version = str(self.code_version),
            options = self.execOptions,
            materials = [mat2dict(m,i) for i,m in enumerate(self.colorList)],
            parameters = [],
            #credits = self.credits
        )
        assert LsystemParameters.validate_schema(result)
        return result     

    @staticmethod
    def validate_schema(obj):
        # TODO: load files only once
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