def splitmodules(text):
    from openalea.lpy import ModuleClass
    it = 0
    result = []
    while text[it] in ' \t' and it < len(text):
        it += 1
    last = it
    while it < len(text):
        c = text[it]
        print(it, c)
        if c.isspace():
            pot = it
            while c.isspace() and it < len(text):
                it += 1
                c = text[it]
            if it == len(text) :
                result.append(text[last:pot])
                last = it
                break
            if c != '(' :
                result.append(text[last:pot])
                last = it

        elif c == '(' :
            nbpar = 0
            while nbpar > 1 or c != ')' and it < len(text):
                if c == '(' : nbpar += 1
                elif c == ')' : nbpar -= 1
                it += 1
                c = text[it]
            assert it != len(text) and 'Cannot find end of arguments'
            it += 1
            if it == len(text) :
                result.append(text[last:it])
                last = it
                break
            c = text[it]
            result.append(text[last:it])
            while c.isspace() and it < len(text):
                it += 1
                c = text[it]
            last = it
        else:
            m = ModuleClass.get(text[last:it+1]) 
            print(repr(text[last:it+1]), m)
            if m is None or m.name == '':
                it += 1
            elif it < len(text)-1 and text[it+1] == '(':
                it += 1
            else:
                result.append(text[last:it+1])
                it += 1
                last = it
    if last != len(text):
        result.append(text[last:])
    return result


modpreference = {'IncColor':'SetColor', 'DecColor':'SetColor',
                 'IncWidth':'SetWidth', 'DecWidth':'SetWidth'}

def translate_production_line(line, keyword = 'nproduce', replacementbeg = 'nsproduce(', replacementend = ')'):    
    from openalea.lpy import ModuleClass
    def checkmod(name, arg):
        if arg != '()' and name in modpreference:
            return modpreference[name]
        else : return name

    if line.strip().startswith(keyword):
        indent = line.index(keyword)
        result = line[:indent]
        result +=  replacementbeg+ ' [ '
        totranslate = line.strip()[len(keyword):]
        comment = ''
        if '#' in totranslate:
            totranslate, comment = totranslate.split('#',1)
        mods = splitmodules(totranslate)
        for i,w in enumerate(mods):
            if '(' in w:
                name, arg = w.split('(',1)
                arg = '('+arg
            else:
                name, arg = w, '()'
            mc = ModuleClass.get(name)
            if mc is None or len(mc.aliases) == 0:
                result += name + arg
            else:
                result += checkmod(mc.aliases[0],arg) + arg
            if i < len(mods)-1: result += ', '
        result += ' ] '
        result += replacementend
        result += comment + line[len(line.rstrip()):]
    else:
        result = line
    return result

def translate(code):
    result = ''
    for line in code.splitlines(True):
        l = line.strip()
        if len(l) > 0:
            firstword =  l.split()[0]
            if firstword == 'nproduce':
                result += translate_production_line(line,'nproduce')
            elif firstword == 'produce':
                result += translate_production_line(line,'produce', 'nsproduce(', '); return')
            elif firstword == 'Axiom:':
                result += translate_production_line(line,'Axiom:', '__axiom__ =', '); return')
            else :
                result += line
        else :
            result += line
    return result