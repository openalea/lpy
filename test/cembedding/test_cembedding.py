
def test_embeddedlpy():
    import os 
    os.system('scons')
    import subprocess
    proc = subprocess.Popen(["./embeddedlpy"], stdout=subprocess.PIPE)
    out, err = proc.communicate()
    out = out.strip()
    print repr(out)
    assert out == "'+(90);(3)I(6765)I(4181)I(2584)I(1597)I(987)I(610)I(377)I(233)I(144)I(89)I(55)I(34)I(21)I(13)I(8)I(5)I(3)I(2)I(1)I(1)'"

if __name__ == '__main__':
    test_embeddedlpy()