import openalea.plantgl.all as pgl
from .. import __lpy_kernel__ as lpykernel  
from collections import OrderedDict
from .scalar import *

default_credits = {'__authors__'    : '' ,
                   '__institutes__'  : '' ,
                   '__copyright__'   : '' ,
                   '__description__' : '' ,
                   '__references__'  : '' }

default_lpycode_version = 1.2
default_lpyjson_version = 1.0

scalartypemap = { int : 'Integer', float :'Float', bool : 'Bool'}
graphictypemap = { pgl.NurbsCurve2D : 'Curve2D', pgl.BezierCurve2D : 'Curve2D', pgl.Polyline2D : 'Curve2D', pgl.NurbsPatch : 'NurbsPatch', pgl.NurbsPatch3D : 'NurbsPatch3D'}

defaultturtlecolorlist = pgl.PglTurtle().getColorList()

def isSimilarToDefaultTurtleMat(cmat, i):
    if cmat.isTexture() : return False

    nbdefault = len(defaultturtlecolorlist)

    if i >= nbdefault:
        defaultmat = pgl.Material('Color_'+str(i))
    else:
        defaultmat = pgl.Material(defaultturtlecolorlist[i])
    if cmat.isSimilar(defaultmat) and cmat.name == defaultmat.name:
        return True
    else:
        return False

class Category:
    def __init__(self, info, items = None, scalars = None, **params):
        if type(info) == str:
            self.info = { 'name' : info, 'enabled' : True}
        else:
            self.info = info
        self.__dict__.update(params)
        self.items = OrderedDict([(m.getName(o),(m,o)) for m,o in items])  if not items is None else OrderedDict()
        self.scalars = OrderedDict([(sc.name,sc) for sc in scalars]) if not scalars is None else OrderedDict()
    def __repr__(self):
        return '<Category('+repr(self.info['name'])+') object at '+hex(id(self))+'>'
    def add_scalar(self, value):
        self.scalars[value.name] = value
    def add_scalars(self, values):
        for v in values:
            self.add_scalar(v)
    def add_item(self, value):
        name = value[0].getName(value[1])
        self.items[name] = value
    def add_items(self, values):
        for v in values:
            self.add_item(v)

class LsystemParameters:
    def __init__(self, lsystem_or_filename = None):
        self.execOptions = {}
        self.animation_timestep = None

        self.color_list = {}

        self.categories = OrderedDict()

        self.credits = default_credits

        self.default_category_name = 'default'

        if not lsystem_or_filename is None:
            if isinstance(lsystem_or_filename, lpykernel.Lsystem):
                self.retrieve_from(lsystem_or_filename)
            else:
                self.load(open(lsystem_or_filename))

    def clear(self):
        self.execOptions = {}
        self.animation_timestep = None
        self.color_list = {}
        self.categories = OrderedDict()
        self.credits = default_credits
        self.default_category_name = 'default'

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

    def check_validity(self):
        from openalea.lpy.gui.abstractobjectmanager import AbstractObjectManager
        assert isinstance(self.execOptions, dict)
        assert isinstance(self.credits, dict)
        if not self.animation_timestep is None:
            assert self.animation_timestep > 0
        assert isinstance(self.color_list, dict)
        for value in self.color_list.values():
            assert isinstance(value, pgl.Appearance)
        for category in self.categories.values():
            assert isinstance(category.info, dict)
            assert 'name' in category.info
            for pmanager, pvalue in category.items.values():
                assert isinstance(pmanager, AbstractObjectManager)
            for value in category.scalars.values():
                assert isinstance(value, BaseScalar)

    def check_similarity(self, other):
        from itertools import zip_longest
        if self.execOptions != other.execOptions:
            raise ValueError('execOptions',self.execOptions,other.execOptions)
        if self.credits != other.credits:
            raise ValueError('credits',self.credits,other.credits)
        if self.animation_timestep != other.animation_timestep:
            raise ValueError('animation_timestep',self.animation_timestep,other.animation_timestep)
        if self.default_category_name != other.default_category_name:
            raise ValueError('default_category',self.default_category_name,other.default_category_name)

        self.check_similar_colors(other)

        self.check_similar_parameters(other)

    def check_similar_parameters(self, other):
        from itertools import zip_longest
        import openalea.plantgl.scenegraph.pglinspect as inspect
        def similar_pgl_object(v1,v2):
            if inspect.is_sceneobject_subclass(v1.__class__) and inspect.is_sceneobject_subclass(v2.__class__):
                attributes = inspect.get_pgl_attributes(v1)
                for att in attributes:
                    if getattr(v1,att) != getattr(v2,att):
                        if not isinstance(getattr(v1,att),pgl.PglObject): # cannot compare easily
                            raise ValueError(att,getattr(v1,att),getattr(v2,att))
                return True
            return True

        for cat1,cat2 in zip_longest(self.categories.values(), other.categories.values()):
            if cat1 is None or cat2 is None:
                raise ValueError('category',cat1,cat2)
            if cat1.info != cat2.info:
                raise ValueError('category',cat1.info,cat2.info)
            if len(cat1.items) != len(cat2.items):
                raise ValueError('category',cat1.items,cat2.items)
            for (m1, v11),(m2,v22) in zip_longest(cat1.items.values(),cat2.items.values()):
                if m1 != m2 :
                    raise ValueError('manager',m1.name,m2.name)
                if type(v11) != type(v22) :
                    raise ValueError('type',v11,v22)
                if not similar_pgl_object(v11,v22) :
                    raise ValueError('object',v11,v22)

            for v1,v2 in zip_longest(cat1.scalars.values(), cat2.scalars.values()):
                if v1.scalartype() != v2.scalartype() or v1.value != v2.value:
                    raise ValueError('scalars',v1,v2)


    def check_similar_colors(self, other):
        from itertools import zip_longest
        for iv1,iv2 in zip_longest(self.color_list.items(), other.color_list.items()):
            if not iv1 is None:
                i1,v1 = iv1
            else:
                v1 = None
            if not iv2 is None:
                i2,v2 = iv2
            else:
                v2 = None
            if v1 is None and not v2 is None :
                if not isSimilarToDefaultTurtleMat(v2,i2):
                    raise ValueError('color',v1,v2)
            elif v2 is None and not v1 is None :
                if not isSimilarToDefaultTurtleMat(v1,i1):
                    raise ValueError(v1,v2)
            else:
                if i1 != i2:
                    raise ValueError('color',i1,i2)
                if type(v1) != type(v2):
                    raise ValueError('color',v1,v2)
                if isinstance(v1, pgl.Material) :
                    if not  v1.isSimilar(v2):
                        raise ValueError('material',v1,v2)
                else:
                    if v1.image.filename != v1.image.filename:
                        raise ValueError('texture',v1.image.filename,v2.image.filename)


    def get_available_parameter_types(self):
        return self.get_available_scalar_types()+self.get_available_graphical_types()

    def get_available_scalar_types(self):
        return scalartypemap.values()

    def get_graphicalparameter_managers(self):
        from openalea.lpy.gui.objectmanagers import get_managers
        return get_managers()

    def get_available_graphical_types(self):
        return list(self.get_graphicalparameter_managers().keys())

    def add(self, name, value, ptype = None, category = None, **params):
        if ptype is None:
            ptype = type(value)
            ptype = scalartypemap.get(ptype,graphictypemap.get(ptype,ptype))
        if ptype in self.get_available_scalar_types():
            self.add_scalar(name, value, ptype, category, **params)
        elif ptype in self.get_available_graphical_types():
            self.add_graphicalparameter(name, value, ptype, category, **params)
        else:
            raise TypeError(ptype)

    def add_category(self, name, **params):
        category = Category({ 'name' : name, 'enabled' : True }, params=params)
        self.categories[name] = category

    def set_defaut_category(self, name):
        self.default_category_name = name

    def add_scalar(self, name, value, ptype = None, category = None, **params):
        if ptype is None:
            ptype = scalartypemap[type(value)]
        if category is None:
            category = self.default_category_name
        assert ptype in self.get_available_scalar_types()
        scalar = ProduceScalar((name, ptype, value))
        scalar.__dict__.update(**params)
        scalar.category = category

        self._add_scalar(category, scalar)
        return scalar

    def _add_scalar(self, category, scalar):
        categoryobj = self.get_category(category)
        categoryobj.add_scalar(scalar)

    def add_function(self, name, value = None, category = None):
        """ if value is None a default function value is created """
        return self.add_graphicalparameter(name, value, 'Function', category)

    def add_curve(self, name, value = None, category = None):
        """ if value is None a default function value is created """
        return self.add_graphicalparameter(name, value, 'Curve2D', category)

    def add_patch(self, name, value = None, category = None):
        """ if value is None a default function value is created """
        return self.add_graphicalparameter(name, value, 'NurbsPatch', category)

    def add_graphicalparameter(self, name, value, ptype = None, category = None):
        assert ptype in self.get_available_graphical_types()
        if ptype is None:
            ptype = graphictypemap[type(value)]
        if category is None:
            category = self.default_category_name
        manager = self.get_graphicalparameter_managers()[ptype]
        if value is None:
            value = manager.createDefaultObject()
        manager.setName(value, name)

        self._add_graphicalparameter(category, manager, value)
        return (manager, value)

    def _add_graphicalparameter(self, category, manager, value):
        categoryobj = self.get_category(category)
        categoryobj.add_item((manager, value))

    def set_option(self, name, value):
        self.execOptions[name] = value

    def get_option(self, name):
        self.execOptions[name]

    def get_options(self):
        return self.execOptions

    def set_color(self, index, value, name = None):
        assert isinstance(value, pgl.Appearance)
        self.color_list[index] = value
        if not name is None:
            value.name = name
        elif not value.isNamed():
            value.name = 'Color_'+str(index)

    def unset_color(self, index):
        assert index in self.color_list
        del self.color_list[index]

    def get_color(self, index):
        return self.color_list.get(index)

    def get_colors(self):
        return self.color_list

    def get_categories(self):
        return self.categories

    def get_category_names(self):
        return self.categories.keys()

    def get_category(self, name = None):
        if name is None:
            name = self.default_category_name
        if not name in self.categories:
            category = Category({ 'name' : name, 'enabled' : True })
            self.categories[name] = category
        return self.categories[name]

    def get_category_info(self, name = None):
        """ If no name are given, the default category info is returned """
        return self.get_category(name).info

    def get_category_parameters(self, name = None) :
        """ If no name are given, the default category parameters are returned """
        return list(self.get_category(name).items.values())+list(self.get_category(name).scalars.values())

    def get_category_graphicalparameters(self, name = None):
        """ If no name are given, the default category graphical parameters are returned """
        return list(self.get_category(name).items.values())

    def get_category_scalars(self, name = None):
        """ If no name are given, the default category scalars are returned """
        return list(self.get_category(name).scalars.values())

    def get_scalars(self):
        return sum([ list(category.scalars.values()) for category in self.categories.values()],[])

    def get_scalar_list(self):
        """ Return the scalar list in the old-fashion with scalar rep of category"""
        result = []
        for category in self.categories.values():
            if len(category.scalars) > 0:
                result.append(CategoryScalar(category.info['name']))
                result += category.scalars.values()
        return result

    def retrieve_from(self, lsystem):
        #self.lsystem = lsystem
        #self.reference_dir = reference_dir
        self.retrieve_from_env(lsystem.execContext())

    def retrieve_from_env(self, context):
        code_version = self._retrieve_code_version_from_env(context)
        self._retrieve_exec_parameters_from_env(context, code_version)
        self._retrieve_colors_from_env(context, code_version)
        self._retrieve_credits_from_env(context, code_version)
        self._retrieve_graphical_parameters_from_env(context, code_version)
        self._retrieve_scalars_from_env(context, code_version)

    def _retrieve_code_version_from_env(self, context):
        return context.get('__lpy_code_version__',default_lpycode_version)

    def _retrieve_credits_from_env(self, context, code_version):
        credits = {}
        for  c in default_credits.keys():
            credits[c] = context.get(c,'')

    def _retrieve_scalars_from_env(self, context, code_version):
        if code_version == 1.1:
            scalars = context.get('__scalars__', [])
            currentcategory = self.default_category_name
            for sc in scalars:
                if sc[1] == 'Category':
                    currentcategory = sc[0]
                else:
                    csc = ProduceScalar(sc)
                    csc.category = currentcategory

                    self._add_scalar(currentcategory, csc)

    def _retrieve_graphical_parameters_from_env(self, context, code_version):
        managers = self.get_graphicalparameter_managers()

        def checkinfo(info):
            if type(info) == str:
                return {'name':info , 'enabled':True}
            else :
                if code_version == 1.1:
                    if 'active' in info:
                        info['enabled'] = info['active']
                        del info['active']
                return info
        parameters = context.get('__parameterset__',[])

        for category in parameters:
            if code_version == 1.1:
                panelinfo, objects = category
            else :
                panelinfo, objects, scalars = category

            info = checkinfo(panelinfo)
            categoryobj = self.categories.setdefault(info['name'], Category(info))
            categoryobj.add_items([(managers[name],obj) for name,obj in objects])
            assert len(categoryobj.items) == len(objects)
            if code_version > 1.1:
                scalars = [scalar_from_json_rep(scalardict) for scalardict in scalars]
                categoryobj.add_scalars(scalars)

    def _retrieve_colors_from_env(self, context, code_version):
        for i,cmat in enumerate(context.turtle.getColorList()):
            if not isSimilarToDefaultTurtleMat(cmat,i):
                self.color_list[i] = cmat

    def _retrieve_exec_parameters_from_env(self, context, code_version):
        self.animation_timestep = None
        if not context.is_animation_timestep_to_default():
            self.animation_timestep = context.animation_timestep
        self.execOptions = {}
        for option in context.options:
            if not option.isToDefault():
                self.execOptions[option.name] = option.selection

    def apply_to(self, lsystem):
        self.apply_to_env(lsystem.execContext())

    def apply_to_env(self, context):
        context["__lpy_code_version__"] = default_lpycode_version
        self._apply_exec_parameters_to_env(context)
        self._apply_colors_to_env(context)
        self._apply_parameters_to_env(context)

    def _apply_exec_parameters_to_env(self, context):
        if not self.animation_timestep is None:
            context.animation_timestep = self.animation_timestep
            context.default_category = self.default_category_name
        for optname, optvalue in self.execOptions.items():
            context.options.setSelection(optname,optvalue)

    def _apply_colors_to_env(self, context):
        for i, cmat in self.color_list.items():
            if not isSimilarToDefaultTurtleMat(cmat, i):
                context.turtle.setMaterial(i, cmat)

    def _apply_parameters_to_env(self, context):
        context["__parameterset__"] = [(category.info,
                                       [(manager.typename, obj) for manager, obj in category.items.values()],
                                       [scalar.todict() for scalar in category.scalars.values()]) for category in self.categories.values()]
        for category in self.categories.values():
            if category.info.get('enabled',True):
                for oname, (manager,obj) in category.items.items():
                    context[oname] = manager.getObjectForLsysContext(obj)
                for scalar in category.scalars.values():
                    context[scalar.name] = scalar.value

    def generate_py_code(self, indentation = '', reference_dir = None, version = default_lpycode_version):
        assert version in [1.1,1.2]
        code = self._generate_main_py_code(indentation, reference_dir,version)
        code += self._generate_credits_py_code(indentation)
        if len(code) > 0:
            code = lpykernel.LpyParsing.InitialisationBeginTag+'\n\n'+'__lpy_code_version__ = '+str(version)+'\n\n'+code
        return code

    def _generate_main_py_code(self, indentation = '', reference_dir = None, version = default_lpycode_version):
        header = indentation+"def "+lpykernel.LsysContext.InitialisationFunctionName+"(context):\n"
        init_txt = self._generate_exec_parameters_py_code(indentation+'\t',version)
        init_txt += self._generate_colors_py_code(indentation+'\t', reference_dir,version)
        init_txt += self._generate_parameters_py_code(indentation+'\t',version)
        init_txt += self._generate_scalars_py_code(indentation+'\t',version)
        if len(init_txt) > 0:
            return header+init_txt
        else:
            return ''

    def _generate_colors_py_code(self, indentation = '\t', reference_dir = None, version = default_lpycode_version):
        if self.color_list is None: return ''

        init_txt = ''
        nbcurrent = len(self.color_list)

        printer = pgl.PyStrPrinter()
        printer.pglnamespace = 'pgl'
        printer.indentation = indentation
        printer.indentation_increment = '\t'
        printer.line_between_object = 0
        if reference_dir and len(reference_dir) > 0:
            printer.reference_dir = os.path.abspath(reference_dir)

        firstcol = True
        for i, cmat in self.color_list.items():
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

    def _generate_exec_parameters_py_code(self, indentation = '\t', version = default_lpycode_version):
        init_txt = ''
        if not self.animation_timestep is None:
            init_txt += indentation+'context.animation_timestep = '+str(self.animation_timestep)+'\n'
        if self.default_category_name != 'default':
            init_txt += indentation+'context.default_category = '+str(self.default_category_name)+'\n'
        for optname, optvalue in self.execOptions.items():
            init_txt += indentation+'context.options.setSelection('+repr(optname)+','+str(optvalue)+')\n'
        return init_txt

    def _generate_parameters_py_code(self, indentation = '\t', version = default_lpycode_version):
        init_txt = ''
        if self.categories is None: return init_txt
        def emptyparameterset(params):
            for category in params.values():
                if len(category.items) > 0: return False
                if len(category.scalars) > 0: return False
            return True
        if not emptyparameterset(self.categories) :
            intialized_managers = {}
            for panelid,category in enumerate(self.categories.values()):
                for manager,obj in category.items.values():
                    if manager not in intialized_managers:
                        intialized_managers[manager] = True
                        init_txt += manager.initWriting('\t')
                    init_txt += manager.writeObject(obj,'\t')
                init_txt += indentation+'category_'+str(panelid)+' = ('+repr(category.info)
                init_txt += ',['+','.join(['('+repr(manager.typename)+','+manager.getName(obj)+')' for manager,obj in category.items.values()])+']\n'
                if version > 1.1:
                    init_txt += ',['+','.join([repr(scalar.todict()) for scalar in category.scalars.values()])+']'
                init_txt += ')\n'
            init_txt += indentation+'parameterset = ['
            init_txt += ','.join(['category_'+str(panelid) for panelid in range(len(self.categories))])
            init_txt += ']\n'
            init_txt += indentation+'context["__parameterset__"] = parameterset\n'
            for category in self.categories.values():
                if category.info.get('enabled',True):
                    for manager,obj in category.items.values():
                        init_txt += indentation+'context["'+manager.getName(obj)+'"] = '+manager.writeObjectToLsysContext(obj) + '\n'
                    for scalar in category.scalars.values():
                        init_txt += indentation+'context["'+scalar.name+'"] = '+str(scalar.value) + '\n'
        return init_txt

    def _generate_scalars_py_code(self, indentation = '\t', version = default_lpycode_version):
        init_txt = ''
        if version == 1.1:
            init_txt += indentation+'scalars = ['+','.join([repr(sc.totuple()) for sc in self.get_scalar_list()])+']\n'
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

        parameters = OrderedDict()
        for category in self.categories.values():
            panel = category.info.copy()
            panel['name']
            panel.setdefault('enabled',True)
            panel['scalars'] = [scalar.todict() for scalar in category.scalars.values()]
            items = []
            for manager,obj in category.items.values():
                items.append(manager.to_json(obj))
            panel['items'] = items
            parameters[panel['name']] = panel

        materials = []
        for i, cmat in self.color_list.items():
            if not isSimilarToDefaultTurtleMat(cmat, i):
                jmat = jrep.to_json_rep(cmat)
                jmat['index'] = i
                materials.append(jmat)

        if self.animation_timestep is None:
            options = self.execOptions
        else:
            options = self.execOptions.copy()
            options['animation_timestep'] = self.animation_timestep
            options['default_category'] = self.default_category_name

        result =  dict(
            schema = 'lpy',
            version = str(default_lpyjson_version),
            options = options,
            materials = materials,
            parameters = list(parameters.values()),
            credits = dict([(key,value) for key,value in self.credits.items() if value != ''])
        )
        assert LsystemParameters.is_valid_schema(result)
        return result

    @staticmethod
    def is_valid_schema(obj):
        # TODO: load files only once
        import io, os, json, jsonschema
        is_valid = False
        schema_path = os.path.join(os.path.dirname(__file__), 'schema')
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
        if not LsystemParameters.is_valid_schema(obj):
            return False
        version = obj.get('version',default_lpyjson_version)
        self.credits.update(obj.get('credits',{}))
        options = obj.get('options',{}).copy()
        if 'animation_timestep' in options:
            self.animation_timestep = options['animation_timestep']
            del options['animation_timestep']
        if 'default_category' in options:
            self.animation_timestep = options['default_category']
            del options['default_category']
        self.execOptions = options

        materials = {}
        for jmat in obj.get('materials',[]):
            idx = jmat['index']
            del jmat['index']
            materials[idx] = jrep.from_json_rep(jmat)
        self.color_list = materials
        parameters = obj.get('parameters',[])
        scalars = []
        gparameters = []
        managers = self.get_graphicalparameter_managers()
        for pset in parameters:
            pinfo = {}
            for propname in pset.keys():
                if not propname in ['items','scalars']:
                    pinfo[propname] = pset[propname]
            pscalars = list(map(scalar_from_json_rep,pset['scalars']))
            for sc in pscalars:
                sc.category = pinfo['name']

            pitems = []
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
                pitems.append((manager,obj))
            category = Category(pinfo, pitems, pscalars)
            self.categories[category.info['name']] = category
        return True

    def loads(self, obj):
        import json
        schema = json.loads(obj)
        self.retrieve_from_json_dict(schema)

    def load(self, file):
        import json
        schema = json.load(file)
        self.retrieve_from_json_dict(schema)

    def dumps(self):
        import json
        obj = self.generate_json_parameter_dict()
        return json.dumps(obj, indent=4)

    def dump(self, file):
        import json
        obj = self.generate_json_parameter_dict()
        json.dump(obj, file, indent=4)
