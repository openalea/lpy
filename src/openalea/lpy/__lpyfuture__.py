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