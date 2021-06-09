from openalea.lpy import helpTurtle,LPY_VERSION_STR
from openalea.plantgl.gui.qt import qt
import os


from openalea.plantgl.gui.qt.QtCore import Qt
from openalea.plantgl.gui.qt.QtGui import QPixmap
from openalea.plantgl.gui.qt.QtWidgets import QMessageBox, QSplashScreen

vplogofilename = ':/logo/biglogo.png'
lpylogofilename = ':/logo/flower.png'

vpInfoTxt = "Fred Boudon et al.\nCIRAD-INRIA-INRA\n"
lpyInfoTxt = "L-Py\nVersion:"+LPY_VERSION_STR+"\nF. Boudon\nhttps://lpy.rtfd.io/"

aboutTxt = """<b>L-Py</b><br>
<it>A <b>Lindenmayer Systems</b><br>&nbsp;&nbsp;framework in <b>Python</b></it>.<br><br>Version :"""+LPY_VERSION_STR+"""<br>
Licence: CeCILL-C<br><br><br><br><br><br><br>
Implemented by F. Boudon et al. <br>Copyright: CIRAD-INRIA-INRA.<br>
<br>
"""

specificationheadertxt = """<H1>L-Py</H1>
<H2> Predefined Symbols </H2>
Here is a recap of the predefined symbol used in L-Py with their turtle interpretation: <BR>

"""

specificationtxt = """
<H2> Predefined commands </H2>
Here comes the python commands that control the simulation.
<H3>The following commands can be redefined to initialize simulation state:</H3>
<table>
<tr><td><b>def Start([lstring])   </b></td><td> is called at the beginning of the simulation. One argument can be optionally defined to receive the input lstring. A modified lstring can be returned by the function to modify the axiom of the simulation.</td></tr>
<tr><td><b>def End([lstring,geometries])     </b></td><td> is called at the end of the simulation. One or two arguments can be optionally defined to receive the final lstring and its geometric interpretation. A modified lstring or scene can be returned by the function to change output of the simulation.</td></tr>
<tr><td><b>def StartEach([lstring]) </b></td><td> is called before each derivation step. One argument can be optionally defined to receive the input lstring. A modified lstring can be returned by the function to modify input lstring of the current iteration.</td></tr>
<tr><td><b>def EndEach([lstring,geometries]) </b></td><td> is called after each derivation step. One or two arguments can be optionally defined to receive the current lstring and its geometric interpretation. Returning an lstring or (lstring, geometries) will be used for next iterations and display. If frameDisplayed() is False, geometries is None.</td></tr>
<tr><td><b>def StartInterpretation()   </b></td><td> is called at the beginning of the interpretation. Interpretable modules can be produced to generate extra graphical elements</td></tr>
<tr><td><b>def EndInterpretation()   </b></td><td> is called at the end of the interpretation. Interpretable modules can be produced to generate extra graphical elements</td></tr>
<tr><td><b>def PostDraw() </b></td><td> is called after drawing the representation of a new lstring.  </td></tr></table>

<H3>Python commands that control the rule application:</H3>
<table>
<tr><td><b>Stop()     </b></td><td> Stop simlation at the end of this iteration.</td></tr>
<tr><td><b>forward()     </b></td><td> Next iteration will be done in forward direction.</td></tr>
<tr><td><b>backward()    </b></td><td> Next iteration will be done in backward direction.</td></tr>
<tr><td><b>isForward()   </b></td><td> Test whether direction is forward.</td></tr>
<tr><td><b>getIterationNb() </b></td><td> Return the id of the current iteration.</td></tr>
<tr><td><b>useGroup(int)    </b></td><td> Next iteration will use rules of given group and default group 0.</td></tr>
<tr><td><b>getGroup()       </b></td><td> Gives which group will be used.</td></tr>
<tr><td><b>frameDisplay(bool)</b></td><td> Set whether a frame will be displayed at the end of the iteration. default is True in animation and False except for last iteration in run mode.</td></tr>
<tr><td><b>isFrameDisplayed() </b></td><td> Tell whether a frame will be displayed at the end of the iteration.</td></tr>
<tr><td><b>isAnimationEnabled() </b></td><td> Return the current simulation is in an animation.</td></tr>
<tr><td><b>requestSelection(caption) </b></td><td> Wait selection in the viewer before next iteration. Set frameDisplay to True.</td></tr>
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
<tr><td><b>nsproduce(<i>LstringStruct</i>) </b></td><td> produce a given <i>Lstring</i> data structure whithout returning.</td></tr>
<tr><td><b>makestring(<i>Lstring</i>) </b></td><td> create an <i>LstringStruct</i> from <i>Lstring</i>.</td></tr>
<tr><td><b>InLeftContext(pattern, argdict) </b></td><td> test a left context. argdict contains value of all parameter of the pattern</td></tr>
<tr><td><b>InRightContext(pattern, argdict) </b></td><td> test a right context. argdict contains value of all parameter of the pattern</td></tr>
<tr><td><b>derivation length: <i>value</i> </b></td><td> number of derivation to do (default=1).</td></tr>
<tr><td><b>initial_view=<i>value</i> </b></td><td> number of derivation for bounding box evaluation (default=derivation length).</td></tr>
<tr><td><b>extern(<i>var<i>=<i>value<i>) </b></td><td> definition of a global variable and its default value that can be redefined externally.</td></tr>
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
L-Py module naming is based on the specification of Lstudio/cpfg-lpfg defined by P. Prusinkiewicz et al. (http://algorithmicbotany.org/lstudio). <BR>
<BR>

For More details, see:
<ul>
<li>  F. Boudon, T. Cokelaer, C. Pradal, P. Prusinkiewicz and C. Godin, L-Py: an L-system simulation framework for modeling plant architecture development based on a dynamic language, Front. Plant Sci., 30 May 2012.</li>
<li>  F. Boudon, T. Cokelaer, C. Pradal and C. Godin, L-Py, an open L-systems framework in Python, FSPM 2010.</li>
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
        if len(parts) == 1:
           return '<tr><td colspan=3 align="center"><H3>'+l+'</H3></td></tr>\n'
        else:
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
    spec = spec.replace('<td colspan=3 align="center"><H3>',"|**__").replace('</H3></td>',"__**|||")
    spec = spec.replace('<td>',"|").replace('</td></tr>',"|").replace('</td>',"").replace('</tr>',"").replace('<tr>',"")
    spec = spec.replace('<H1>',"======").replace('</H1>',"======")
    spec = spec.replace('<H2>',"=====").replace('</H2>',"=====")
    spec = spec.replace('<H3>',"").replace('</H3>',"")
    spec = spec.replace('<table>',"").replace('</table>',"")
    return spec
    
def aboutLpy(parent):
    lpyDialog(parent)
    #QMessageBox.about(parent,"LPy",aboutTxt)
        
        
def splashLPy(): 
    try:
        return lpyDialog()
    except Exception as e:
        print(e)
        pass

     
def lpyDialog(parent = None):
    if not parent or not hasattr(parent,'splash'):
        #if not os.path.exists(logofilename): raise Exception('No logo image')
        pix = QPixmap(lpylogofilename)
        splash = QSplashScreen(pix)
    else:
        splash = parent.splash
    #splash.showMessage(lpyInfoTxt,Qt.AlignBottom|Qt.AlignLeft)
    splash.showMessage(aboutTxt,Qt.AlignBottom|Qt.AlignLeft)
    splash.show()
    if parent:
        parent.splash = splash
    return splash
        
