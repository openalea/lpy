def get_filename(fname, pymodname = __file__):
    import os
    return os.path.join(os.path.dirname(pymodname), fname)
