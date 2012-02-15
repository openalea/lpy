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
    if endeach.func_code.co_argcount == 0:
       if len(args) < 1 : raise ValueError('EndEach should be created with lstring as argument')
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
    return wrapped
