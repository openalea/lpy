import sys

def getMaxCharByCol(table):
    max = []

    for row in table:
        index = 0

        for col in row:
            if len(max) <= index:
                max.append(len(col))
            elif len(col) > max[index]:
                max[index] = len(col)
            index += 1
    return max

def printRowBorder(outfile, size):
    outfile.write("+")
    for s in size:
        for i in range(s): outfile.write("-")
        outfile.write("+")
    outfile.write("\n")

def printTitle(outfile, str):
    outfile.write("\n" + str + "\n")
    for i in range(len(str)):
        outfile.write("#")
    outfile.write("\n")

def printSubTitle(outfile, str):
    outfile.write("\n" + str + "\n")
    for i in range(len(str)):
        outfile.write("=")
    outfile.write("\n")

def printSection(outfile, str):
    outfile.write("\n" + str + "\n")
    for i in range(len(str)):
        outfile.write("-")
    outfile.write("\n")

def changeBalisOnString(str):
    str = str.replace("<i>", "*")
    str = str.replace("</i>", "*")
    str = str.replace("<b>", "**")
    str = str.replace("</b>", "**")

    return str

def interpretTable(outfile, table):
    sizeCol = getMaxCharByCol(table)
    outfile.write("\n")
    printRowBorder(outfile, sizeCol)

    for row in table:
        outfile.write("|")
        index = 0

        for col in row:
            outfile.write(col)

            res = sizeCol[index] - (len(col))

            for r in range(res): outfile.write(" ")

            outfile.write("|")
            index += 1
        outfile.write("\n")
        printRowBorder(outfile, sizeCol)

def readTable(outfile, infile):

    table = []
    numCol = -1

    while True:
        l = infile.readline()
        if len(l) == 0:
            break

        l = l.replace("\n", "")
        if l == ".table-end:":
            break

        row = []
        parts = l.split("{")
        numColPart = 0
        for p in parts:
            if len(p) > 0 and not l.isspace():
                numColPart += 1
        if numCol == -1:
            numCol = numColPart
        elif numColPart != numCol:
            exit(42)

        for p in parts:
            if len(p) > 0 and not l.isspace():
                endCol = p.find("}")
                if endCol == -1:
                    exit(42)
                row.append(changeBalisOnString(p[:endCol]))
        table.append(row)
    interpretTable(outfile, table)

def readList(outfile, infile):
    outfile.write("\n")
    while True:
        l = infile.readline()
        if len(l) == 0:
            break

        l = l.replace("\n", "")
        if l == ".list-end:":
            break
        outfile.write("- " + changeBalisOnString(l) + "\n")
    pass

def interpretFile(outfile, fileName):
    infile = open(fileName, "r")

    while True:
        l = infile.readline()
        if len(l) == 0:
            break

        l = l.replace("\n", "")

        if len(l) > 0 and not l.isspace():
            if l[0] == ".":
                parts = l.split(":", 1)
                comm = parts[0][1:]
                if comm == "title" or comm == "sub-title" or comm == "section":
                    if len(parts) == 1:
                        exit(42)

                    quote_begin = parts[1].find("\"")
                    quote_end = parts[1].rfind("\"")
                    if quote_begin == -1 or quote_end == -1:
                        exit(42)
                    if comm == "title":
                        printTitle(outfile, parts[1][quote_begin + 1:quote_end])
                    elif comm == "sub-title":
                        printSubTitle(outfile, parts[1][quote_begin + 1:quote_end])
                    else:
                        printSection(outfile, parts[1][quote_begin + 1:quote_end])
                elif comm == "table-begin":
                    readTable(outfile, infile)
                elif comm == "list-begin":
                    readList(outfile, infile)
                else:
                    exit(45)
            else:
                outfile.write(changeBalisOnString(l) + "\n")

def interpretHelpTurtle(outfile):
    ht = helpTurtle()
    table = []

    for l in ht.split('\n'):
        if len(l) == 0: outfile.write("\n")
        parts = l.split(':', 1)
        if len(parts) == 1:
            if len(table) > 0:
                interpretTable(outfile, table)
                table = []

            outfile.write("\n" + parts[0] + "\n")
            for i in range(len(parts[0])):
                outfile.write("-")
            outfile.write("\n")

        else:
            assert len(parts) == 2

            row = []
            if '(' in parts[0]:
                nsplits = parts[0].split('(')
                row.append(nsplits[0])
                row.append(nsplits[1].split(')')[0])
            else:
                row.append(parts[0])
                row.append("")
            row.append(parts[1])
            table.append(row)

outfile = open(sys.argv[1], "w")

try:
    from openalea.lpy import helpTurtle
    interpretFile(outfile, "header.helpcard")
    interpretHelpTurtle(outfile)
    interpretFile(outfile, "predef_com.helpcard")
except:
    pass

