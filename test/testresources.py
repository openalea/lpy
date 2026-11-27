from contextlib import contextmanager


def get_filename(fname, pymodname = __file__):
    import os
    return os.path.join(os.path.dirname(pymodname), fname)


@contextmanager
def active_context(context):
    context.makeCurrent()
    try:
        yield
    finally:
        context.done()


def assert_axial_tree(lsystem, result, expected):
    from openalea.lpy import AxialTree

    with active_context(lsystem.context()):
        assert result == AxialTree(expected)
