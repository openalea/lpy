

class Scalar:
    def __init__(self,name,value = 1,minvalue = 0, maxvalue = 100):
        self.name = name
        self.value = value
        self.minvalue = minvalue
        self.maxvalue = maxvalue
        
    def importValue(self,other):
        self.name = other.name
        self.value = other.value
        self.minvalue = other.minvalue
        self.maxvalue = other.maxvalue
    