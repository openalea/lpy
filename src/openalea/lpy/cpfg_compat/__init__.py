from random import uniform

def select_successor_from_prob(parray):
    totprob = sum(parray)
    p0 = uniform(0,totprob)
    for i,pi in enumerate(parray):
        if p0 < pi:
            return i
        else: p0 -= pi
    return len(parray)