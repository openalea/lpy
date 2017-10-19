from openalea.lpy import helpTurtle
import sys

ht = helpTurtle()
outfile = open(sys.argv[1], "w")

for l in ht.split('\n'):
    print l