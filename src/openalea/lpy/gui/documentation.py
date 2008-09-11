from openalea.lpy import helpTurtle,LPY_VERSION_STR
from PyQt4.QtCore import Qt
from PyQt4.QtGui import QPixmap,QSplashScreen,QMessageBox
import os

logofilename = ':/logo/biglogo.png'

infoTxt = "Virtual Plants Team.\nCIRAD-INRIA-INRA\nSee:http://www-sop.inria.fr/virtualplants/"

aboutTxt = """<b>L-Py</b><br> A Python version of <b>Lindenmayer Systems</b>.<br>
Version :"""+LPY_VERSION_STR+"""<br>
Based on P. Prusinkiewicz et al. Lstudio/cpfg-lpfg specifications.<br>
Implemented by F. Boudon for Virtual Plants.<br>See:http://www-sop.inria.fr/virtualplants/
"""

specificationheadertxt = """<H1>L-Py</H1>
L-Py is based on the specification of Lstudio/cpfg-lpfg defined by P. Prusinkiewicz et al. (http://algorithmicbotany.org/lstudio). 
<H2> Predefined Symbols </H2>
Here is a recap of the predefined symbol used in L-Py with their turtle interpretation: <BR>

"""

specificationtxt = """
<H2> Predefined commands </H2>
Here comes the python commands that control the simulation.
<H3>The following commands can be redefined to initialize simulation state:</H3>
<table>
<tr><td><b>def Start()   </b></td><td> is called at the beginning of the simulation.</td></tr>
<tr><td><b>def End()     </b></td><td> is called at the end of the simulation.</td></tr>
<tr><td><b>def StartEach() </b></td><td> is called before each derivation step.</td></tr>
<tr><td><b>def EndEach([lstring,scene]) </b></td><td> is called at the end of the simulation. One or two arguments can be optionally defined to receive the current lstring and its geometric interpretation. </td></tr>
</table>

<H3>Python commands that control the rule application:</H3>
<table>
<tr><td><b>forward()     </b></td><td> Next iteration will be done in forward direction.</td></tr>
<tr><td><b>backward()    </b></td><td> Next iteration will be done in backward direction.</td></tr>
<tr><td><b>isForward()   </b></td><td> Test whether direction is forward.</td></tr>
<tr><td><b>getIterationNb() </b></td><td> Return the id of the current iteration.</td></tr>
<tr><td><b>useGroup(int)    </b></td><td> Next iteration will use rules of given group and default 0 group.</td></tr>
<tr><td><b>getGroup()       </b></td><td> Gives which group will be used.</td></tr>
</table>

<H3>Lpy specific declaration: </H3>
<table>
<tr><td><b>module <i>name</i>    </b></td><td> declaration of module name.</td></tr>
<tr><td><b>consider: <i>name</i> </b></td><td> symbol to consider.</td></tr>
<tr><td><b>ignore: <i>name</i>   </b></td><td> symbol to ignore.</td></tr>
<tr><td><b>group <i>id</i>:      </b></td><td> following rules will be associated to group <i>id</i>.</td></tr>
<tr><td><b>Axiom: <i>Lstring</i> </b></td><td> declaration of the axiom of the Lsystem</td></tr>
<tr><td><b>produce <i>Lstring</i> </b></td><td> produce an <i>Lstring</i> and return.</td></tr>
<tr><td><b>nproduce <i>Lstring</i> </b></td><td> produce an <i>Lstring</i> whithout returning.</td></tr>
<tr><td><b>derivation length: <i>value</i> </b></td><td> number of derivation to do (default=1).</td></tr>
<tr><td><b>production:     </b></td><td> start of the production rules declaration.</td></tr>
<tr><td><b>homomorphism:   </b></td><td> start of the interpretation rules declaration.</td></tr>
<tr><td><b>interpretation: </b></td><td> start of the interpretation rules declaration.</td></tr>
<tr><td><b>decomposition:  </b></td><td> start of the decomposition rules declaration.</td></tr>
<tr><td><b>maximum depth:  </b></td><td> number of decomposition or interpretation recursive call to do (default=1).</td></tr>
<tr><td><b>endgroup        </b></td><td> reactivate default group 0.</td></tr>
<tr><td><b>endlsystem      </b></td><td> end of lsystem rules declaration.</td></tr>
</table>

<H3>These commands have been added to the original cpfg-lpfg specification:</H3>
<table>
<tr><td><b>context()     </b></td><td> Get context of execution of the L-system. To use with care.</td></tr>
</table>

<H3>The following objects and commands are also accessible from within the lpy shell:</H3>
<table>
<tr><td><b>lstring     </b></td><td> contains the last computed lsystem string of the current simulation.</td></tr>
<tr><td><b>lsystem     </b></td><td> reference to the internal lsystem object representing the current simulation.</td></tr>
<tr><td><b>window   </b></td><td> reference to lpy widget object.</td></tr>
<tr><td><b>clear()   </b></td><td> to clear the shell.</td></tr>
</table>
<BR><BR>
All these functions are imported from openalea.lpy module. Other data structures and functionnalities are available in the module. You can check them with <b>help(openalea.lpy)</b>.<BR>

<H2> References </H2>
For More details, see:
<ul>
<li>  P. Prusinkiewicz et al., 89, The algorithmic Beauty of Plants, Springer-Verlag.</li>
<li>  P. Prusinkiewicz. Graphical applications of L-systems. Proceedings of Graphics Interface '86, pp. 247-253.</li>
<li>  P. Prusinkiewicz, R. Karwowski, and B. Lane. The L+C plant modelling language. In Functional-Structural Plant Modelling in Crop Production, J. Vos et al. (eds.), Springer, 2007.</li>
</ul>


"""

# <tr><td><b>setSelectionRequired(bool)</b></td><td> Specify that a selection in the visualisation can occur
# and should be treated with the insertion of a X module in the string.</td></tr>
# <tr><td><b>isSelectionRequired() </b></td><td> Gives whether selection in the visualisation should be treated.</td></tr>


def getSpecification():
    def lineprocess(l):
        if len(l) == 0: return ''
        parts = l.split(':',1)
        assert len(parts) == 2
        if '(' in parts[0]:
            nsplits = parts[0].split('(')
            modname =  '<td><b>'+nsplits[0]+'</b></td><td><b>'+nsplits[1].split(')')[0]+'</b></td>'
        else:
            modname =  '<td><b>'+parts[0]+'</b></td><td></td>'
        return '<tr>'+modname+'<td>'+parts[1]+'</td></tr>\n'
    ht = helpTurtle()
    
    ht = '<table>'+''.join([lineprocess(l) for l in ht.split('\n')])+'</table>'
    #ht = ht.replace('\n','<BR>')
    txt = specificationheadertxt+ht+specificationtxt
    return txt

def getWikiSpecification():
    spec = getSpecification()
    spec = spec.replace('<b>',"''").replace('</b>',"''")
    spec = spec.replace('<i>',"//").replace('</i>',"//")
    spec = spec.replace('<td>',"|").replace('</td></tr>',"|").replace('</td>',"").replace('</tr>',"").replace('<tr>',"")
    spec = spec.replace('<H1>',"======").replace('</H1>',"======")
    spec = spec.replace('<H2>',"=====").replace('</H2>',"=====")
    spec = spec.replace('<H3>',"").replace('</H3>',"")
    spec = spec.replace('<table>',"").replace('</table>',"")
    return spec
    
def aboutLpy(parent):
    QMessageBox.about(parent,"LPy",aboutTxt)
        
def aboutVPlants(parent): 
        #try:
        vplantsDialog(infoTxt,parent)
        #except:
        #QMessageBox.about(parent,"Virtual Plants",infoTxt)
        
def splashLPy(): 
    try:
        return vplantsDialog("<b>L-Py - "+LPY_VERSION_STR+"</b>")
    except:
        pass

def vplantsDialog(txt, parent = None):
    if parent and not hasattr(parent,'splash'):
        #if not os.path.exists(logofilename): raise Exception('No logo image')
        pix = QPixmap(logofilename)
        splash = QSplashScreen(pix)
    else:
        splash = parent.splash
    splash.showMessage(infoTxt,Qt.AlignBottom|Qt.AlignLeft)
    splash.show()
    if parent:
        parent.splash = splash
    return splash
        
