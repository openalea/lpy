from random import uniform, normalvariate, betavariate, seed

ran = lambda x : uniform(0,x)
srand = seed
nran = lambda m,s : normalvariate(m,s)
bran = lambda a,b : betavariate(a,b)
# biran 


def sin(a):
    from math import sin, radians
    return sin(radians(a))

def cos(a):
    from math import cos, radians
    return cos(radians(a))

def tan(a):
    from math import tan, radians
    return tan(radians(a))

def acos(x):
    from math import acos, degrees
    return degrees(acos(x))

def asin(x):
    from math import asin, degrees
    return degrees(asin(x))

def atan(x):
    from math import atan, degrees
    return degrees(atan(x))

def atan2(x):
    from math import atan2, degrees
    return degrees(atan2(x))

from math import exp, log, sqrt
fabs = abs

from math import floor, ceil, trunc

from numpy import sign

def func(f,v):
    print('test',v)
    return f(min(1,max(0,v)))
