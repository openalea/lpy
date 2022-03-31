def parse_config_file(ctext):
    config = {}
    for cline in ctext.splitlines():
        cline = cline.strip()
        if len(cline) > 1 and cline[:2] != '/*':
            key, value = cline.split(':',1)
            if key in config:
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
    if 'angle factor' in vconfig:
        angle_increment = 360 / float(vconfig['angle factor'])
    if 'angle increment' in vconfig:
        angle_increment = float(vconfig['angle increment'])
    if angle_increment:
        vlpyconfig['AngleIncrement'] = angle_increment
    
    if 'initial color' in vconfig:
        vlpyinitconfig['initial_color'] = int(vconfig['initial color'].split()[0])
    
    if 'color increment' in vconfig:
        vlpyconfig['ColorIncrement'] = int(vconfig['color increment'].split()[0])
    
    unit = None
    if 'initial line width' in vconfig:
        try:
            val, unit = vconfig['initial line width'].split()
        except:
            val = vconfig['initial line width']
            unit = None

        vlpyinitconfig['initial_line_width'] = float(val)
        if unit == 'p' or unit == 'pixels':
            vlpyinitconfig['initial_line_width'] /= 10
    
    if 'line width increment' in vconfig:
        vlpyconfig['WidthIncrement'] = float(vconfig['line width increment'])
        if unit == 'p' or unit == 'pixels':
            vlpyconfig['WidthIncrement'] /= 10

    if 'initial scale' in vconfig:
        vlpyinitconfig['initial_scale'] = float(vconfig['initial scale'])
        
    if 'scale multiplier' in vconfig:
        vlpyconfig['ScaleMultiplier'] = float(vconfig['scale multiplier'])
    
    if 'tropism direction' in vconfig:
        vlpyconfig['tropism_direction'] = list(map(float,vconfig['tropism direction'].split(',')))
        
    if 'initial elasticity' in vconfig:
        vlpyinitconfig['initial_elasticity'] = float(vconfig['initial elasticity'])
    
    if 'elasticity increment' in vconfig:
        vlpyconfig['ElasticityIncrement'] = float(vconfig['elasticity increment'])
    
    surfaces = None
    if 'surface' in vconfig:
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
    if  'initial_color' in vlpyinitconfig:
        res += ',('+str(vlpyinitconfig['initial_color'])+')'
    if  'initial_line_width' in vlpyinitconfig:
        res += '_('+str(vlpyinitconfig['initial_line_width'])+')'
    if  'initial_scale' in vlpyinitconfig:
        res += '@D('+str(vlpyinitconfig['initial_scale'])+')'
    if  'initial_elasticity' in vlpyinitconfig:
        res += '@Ts('+str(vlpyinitconfig['initial_elasticity'])+')'
    return res

def generate_view_code(vlpyconfig,groupofpatches = None):
    import os.path
    code = '# Turtle attributes \n'
    for att in ['AngleIncrement','ColorIncrement','WidthIncrement','ScaleMultiplier']:
        if att in vlpyconfig:
            code += 'context().turtle.set'+att+'('+str(vlpyconfig[att])+')\n'
    if 'tropism direction' in vlpyconfig:
        val = vlpyconfig['tropism direction']
        code += 'context().turtle.setTropism('+str((val[0],val[2],val[1]))+')\n'
        
    if groupofpatches and len(groupofpatches) > 0:
        code += '# Patch gathering \n'
        code += 'import openalea.plantgl.all as pgl\n'
        for group,patches in groupofpatches.items():
            code += group+' = pgl.Group(['+','.join(patches)+'])\n'
            code += group+'.name = '+repr(group)+'\n'
    if 'surfaces' in vlpyconfig:
        code += '# Patch declaration in turtle \n'
        for name, value in vlpyconfig['surfaces'].items():
            fname,scale = value
            code += 'context().turtle.setSurface('+repr(name)+',pgl.Scaled('+str(scale)+','+os.path.splitext(fname)[0]+'))\n'
    return code
    
def translate_view_code(vtext,groupofpatches = None):
    vconfig = parse_config_file(vtext)
    vlpyconfig, vlpyinitconfig = get_view_info(vconfig)
    return generate_view_code(vlpyconfig,groupofpatches), vlpyinitconfig
    
def translate_view_file(vfile,groupofpatches = None):
    vtext = open(vfile,'r').read()
    return translate_view_code(vtext,groupofpatches)
