def parse_config_file(ctext):
    config = {}
    for cline in ctext.splitlines():
        cline = cline.strip()
        if len(cline) > 1 and cline[:2] != '/*':
            key, value = cline.split(':',1)
            if config.has_key(key):
                nvalue = config[key]
                if type(nvalue) != list: nvalue = [nvalue]
                nvalue.append(value)
                value = nvalue
            config[key] = value
    return config

def get_view_info(vconfig):
    vlpyconfig = {}
    vlpyinitconfig = {}
    
    angle_increment = None
    if vconfig.has_key('angle factor'):
        angle_increment = 360 / float(vconfig['angle factor'])
    if vconfig.has_key('angle increment'):
        angle_increment = float(vconfig['angle increment'])
    if angle_increment:
        vlpyconfig['AngleIncrement'] = angle_increment
    
    if vconfig.has_key('initial color'):
        vlpyinitconfig['initial_color'] = int(vconfig['initial color'].split()[0])
    
    if vconfig.has_key('color increment'):
        vlpyconfig['ColorIncrement'] = int(vconfig['color increment'].split()[0])
    
    unit = None
    if vconfig.has_key('initial line width'):
        val,unit = vconfig['initial line width'].split()
        vlpyinitconfig['initial_line_width'] = float(val)
        if unit == 'p' or unit == 'pixels':
            vlpyinitconfig['initial_line_width'] /= 10
    
    if vconfig.has_key('line width increment'):
        vlpyconfig['WidthIncrement'] = float(vconfig['line width increment'])
        if unit == 'p' or unit == 'pixels':
            vlpyconfig['WidthIncrement'] /= 10

    if vconfig.has_key('initial scale'):
        vlpyinitconfig['initial_scale'] = float(vconfig['initial scale'])
        
    if vconfig.has_key('scale multiplier'):
        vlpyconfig['ScaleMultiplier'] = float(vconfig['scale multiplier'])
    
    if vconfig.has_key('tropism direction'):
        vlpyconfig['tropism_direction'] = map(float,vconfig['tropism direction'].split(','))
        
    if vconfig.has_key('initial elasticity'):
        vlpyinitconfig['initial_elasticity'] = float(vconfig['initial elasticity'])
    
    if vconfig.has_key('elasticity increment'):
        vlpyconfig['ElasticityIncrement'] = float(vconfig['elasticity increment'])
    
    surfaces = None
    if vconfig.has_key('surface'):
        values = vconfig['surface']
        if type(values) != list: values = [values]
        surfaces = dict()
        for value in values:
            vparts = value.split()
            surfaces[vparts[0]] = (vparts[1],vparts[2]) # label = (filename,scale)
        vlpyconfig['surfaces'] = surfaces
    
    return vlpyconfig, vlpyinitconfig

def translate_view_init(vlpyinitconfig):
    res = ''
    if  vlpyinitconfig.has_key('initial_color'):
        res += ',('+str(vlpyinitconfig['initial_color'])+')'
    if  vlpyinitconfig.has_key('initial_line_width'):
        res += '_('+str(vlpyinitconfig['initial_line_width'])+')'
    if  vlpyinitconfig.has_key('initial_scale'):
        res += '@D('+str(vlpyinitconfig['initial_scale'])+')'
    if  vlpyinitconfig.has_key('initial_elasticity'):
        res += '@Ts('+str(vlpyinitconfig['initial_elasticity'])+')'
    return res

def generate_view_code(vlpyconfig,groupofpatches = None):
    import os.path
    code = '# Turtle attributes \n'
    for att in ['AngleIncrement','ColorIncrement','WidthIncrement','ScaleMultiplier']:
        if vlpyconfig.has_key(att):
            code += 'context().turtle.set'+att+'('+str(vlpyconfig[att])+')\n'
    if vlpyconfig.has_key('tropism direction'):
        val = vlpyconfig['tropism direction']
        code += 'context().turtle.setTropism('+str((val[0],val[2],val[1]))+')\n'
        
    if groupofpatches and len(groupofpatches) > 0:
        code += '# Patch gathering \n'
        code += 'import openalea.plantgl.all as pgl\n'
        for group,patches in groupofpatches.iteritems():
            code += group+' = pgl.Group(['+','.join(patches)+'])\n'
            code += group+'.name = '+repr(group)+'\n'
    if vlpyconfig.has_key('surfaces'):
        code += '# Patch declaration in turtle \n'
        for name, value in vlpyconfig['surfaces'].iteritems():
            fname,scale = value
            code += 'context().turtle.setSurface('+repr(name)+',pgl.Scaled('+str(scale)+','+os.path.splitext(fname)[0]+'))\n'
    return code
    
def translate_view_code(vtext,groupofpatches = None):
    vconfig = parse_config_file(vtext)
    vlpyconfig, vlpyinitconfig = get_view_info(vconfig)
    return generate_view_code(vlpyconfig,groupofpatches), vlpyinitconfig
    
def translate_view_file(vfile,groupofpatches = None):
    vtext = file(vfile,'r').read()
    return translate_view_code(vtext,groupofpatches)
