
def get_caller_frame():
    import inspect
    return inspect.getouterframes(inspect.currentframe())[2][0]
   
class LocalsRetriever:
    def __init__(self, func):
        self.mylocals = {}
        self.depth = 0
        self.func = func

    def _mytracer(self, frame, event, arg):
        #print frame, event, arg
        if event == 'call':
            self.depth += 1
            if self.depth == 1:
                return self._mytracer
        elif event == 'return':
            self.depth -= 1
            self.mylocals.update(frame.f_locals)
        elif event == 'line':
            pass
        elif event == 'exception':
            pass
        else:
            pass

    def __call__(self, *args, **kwds):
        import sys
        sys.settrace(self._mytracer)
        res = None
        try:
            res = self.func(*args, **kwds)
        finally:
            sys.settrace(None)
        return self.mylocals, res


def __default_parameters_wrapper(function):
    def wrapper(*args, **kwargs):
        l = LocalsRetriever(function)
        params, res = l(*args,**kwargs)
        list(map(get_caller_frame().f_locals.setdefault, list(params.keys()), list(params.values())))
        return res
    return wrapper

def defaultparameters(function):
    """ 
    A decorator to define in a simple way default values of parameters.

    This decorator retrieve all the local variables of a function and insert them
    in the global namespace if they do not already exist after the function definition. 

    Example:

    @defaultparameters
    def myparams():
        a = 1
        b = 2

    c  = a * b

    If the function has parameters, it should be called explicitly to have initialization.
    @defaultparameters
    def myparams(i):
        a = 1
        b = 2*i
    
    myparams(4)
    c  = a * b


    """
    import inspect
    if len(inspect.getargspec(function).args) == 0:
        params, res = LocalsRetriever(function)()
        list(map(get_caller_frame().f_locals.setdefault, list(params.keys()), list(params.values())))
        return function
    else:
        return __default_parameters_wrapper(function)


def extern(*paramstocheck, **params):
    """ 
    A function to define in a simple way default values of parameters.

    This function insert the parameter define as arguments
    in the global namespace if they do not already exist. 

    If a string name is given without argument, 
    it will simply check if the name exists in the current namespace.

    Example:

    extern(a = 1, b = 2) # set default value to variable a and b.
    extern('c') # check existence of variable c.

    """
    caller_frame_locals = get_caller_frame().f_locals
    if len(params) > 0 :
        for key,val in params.items():            
            caller_frame_locals.setdefault(key,val)
            # Mark parameter to be external
            caller_frame_locals.setdefault('__externs__',set()).add(key)
    if len(paramstocheck) > 0:
        for val in paramstocheck:
            assert(type(val) == str)
            if not val in caller_frame_locals:
                raise NameError(val,'should be provided by external parameters.')
            caller_frame_locals.setdefault('__externs__',set()).add(val)






__all__ = ['defaultparameters','extern']