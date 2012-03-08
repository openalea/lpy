import openalea.lpy as lpy

__string_discarded = False
__last_string = None

def DiscardNewString():
  global __string_discarded
  __string_discarded = True

def __check_discard_string(lstring):
   global __string_discarded, __last_string
   if __string_discarded:
     __string_discarded = False
     return __laststring
   __last_string = lpy.Lstring(lstring)

def enable_string_discard(endeach):
    endeach_arg_nb = endeach.func_code.co_argcount
    if endeach_arg_nb == 0:
        def wrapped(lstring):
           global __string_discarded, __last_string
           if __string_discarded:
             __string_discarded = False
             return __last_string
           res = endeach()
           if __string_discarded:
             __string_discarded = False
             return __last_string
           __last_string = lpy.Lstring(lstring)
           return res
        return wrapped
    else:
        def wrapped(lstring, *args):
           global __string_discarded, __last_string
           if __string_discarded:
             __string_discarded = False
             return __last_string
           res = endeach(lstring, *args)
           if __string_discarded:
             __string_discarded = False
             return __last_string
           __last_string = lpy.Lstring(lstring)
           return res
        
        if endeach_arg_nb == 1:   return wrapped
        elif endeach_arg_nb == 2: return lambda lstring, lscene : wrapped(lstring, lscene)
        else:
            raise ValueError('EndEach should ne defined with maximum 2 arguments.')

@enable_string_discard
def EndEach(): pass

__push_string =  None
__pop_string  =  None
__string_stack = { }

def pushString( stringname ):
    global __push_string
    __push_string = stringname

def popString( stringname ):
    global __pop_string
    if not __string_stack.has_key(stringname):
        raise ValueError('Stack of string has not string named '+repr(stringname))
    __pop_string = stringname
    return __string_stack[stringname]
    
def enable_string_pushpop(endeach):
    endeach_arg_nb = endeach.func_code.co_argcount
    def check_push_pop(lstring, res):
        global __push_string, __pop_string, __string_stack
        if not __push_string is None:
            __string_stack[__push_string] = lpy.Lstring(lstring)
            __push_string = None
        if not __pop_string is None:
            oldlstring = __string_stack[__pop_string]
            del __string_stack[__pop_string]
            __pop_string = None
            return oldlstring
        return res
    if endeach_arg_nb == 0:
        def wrapped(lstring):
            return check_push_pop(lstring,endeach())
    elif endeach_arg_nb == 1: 
        def wrapped(lstring):
            return check_push_pop(lstring,endeach(lstring))
    elif endeach_arg_nb == 2: 
        def wrapped(lstring, lscene):
            return check_push_pop(lstring,endeach(lstring,lscene))
    else:
        raise ValueError('EndEach should ne defined with maximum 2 arguments.')
    return wrapped
