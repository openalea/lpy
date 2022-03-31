template_beg = """
{
  "metadata": {
    "type": "tree",
    "origin": "lpy",
    "axial": true,
    "size": %i
  },
  "object": {
"""
template_end = """
  }
}
"""

template_unit = """
    "%i": {
      "children id": %s,
      "parent id": "%s",
      "successor": %s,
      "lstring_attribute": {
%s
      }
    },
"""

def export_attribute(module, indentation = '        '):
    result = indentation + '"name" : "'+module.name+'",\n'
    namedParameters = {}
    module.getNamedParameters(namedParameters)
    for name, value in namedParameters.items():
        result += indentation + repr(name)+' : '+repr(value) + ',\n'
    if len(module) > module.getNamedParameterNb():
        for i, value in enumerate(module[module.getNamedParameterNb():]):
            result += indentation + '"unamed_'+str(i)+'" : ' + repr(value) + ',\n'
    return result

def export_to_json(lstring, fname, toignore = '+-/\\', toconsider = None):
    from openalea.lpy import ConsiderFilter
    stream = open(fname, 'w')
    stream.write(template_beg % len(lstring))
    c = ConsiderFilter.consider(toconsider) if not toconsider is None else ConsiderFilter.ignore(toignore)

    for i,module in enumerate(lstring):
        if not module.isBracket() :
            parent = lstring.parent(i, c)
            children = lstring.children(i, c)
            if children is None : children = []
            elif type(children) is int : children = [children]
            stream.write(template_unit % (i, 
                        repr(children), repr(parent), 'true' if parent is None or lstring.direct_child(parent, c) == i else 'false', export_attribute(module) ))

    stream.write(template_end)


def test():
    import openalea.lpy as lpy
    l = lpy.Lstring('FF(3)[+F][-(80)F]F')
    export_to_json(l, 'test.json')

if __name__ == '__main__':
    test()