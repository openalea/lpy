L-Py Helpcard
=============

L-Py-card
L-Py is based on the specification of Lstudio/cpfg-lpfg defined by P. Prusinkiewicz et al. (http://algorithmicbotany.org/lstudio).
Predefined Symbols
Here is a recap of the predefined symbol used in L-Py with their turtle interpretation:

.. raw:: html
	<H1>L-Py</H1>
	L-Py is based on the specification of Lstudio/cpfg-lpfg defined by P. Prusinkiewicz et al. (http://algorithmicbotany.org/lstudio).
	<H2> Predefined Symbols </H2>
	Here is a recap of the predefined symbol used in L-Py with their turtle interpretation: <BR>

	<table><tr><td colspan=3 align="center"><H3>Structure</H3></td></tr>
	<tr><td><b>[ </b></td><td><b> SB </b></td><td> Push the state in the stack.</td></tr>
	<tr><td><b>] </b></td><td><b> EB </b></td><td> Pop last state from turtle stack and make it the its current state.</td></tr>
	<tr><td colspan=3 align="center"><H3>Rotation</H3></td></tr>
	<tr><td><b>Pinpoint </b></td><td></td><td> Orient turtle toward (x,y,z) . Params : 'x, y, z' or 'v' (optionals, default = 0).</td></tr>
	<tr><td><b>PinpointRel </b></td><td></td><td> Orient turtle toward pos+(x,y,z) . Params : 'x, y, z' or 'v' (optionals, default = 0).</td></tr>
	<tr><td><b>@R </b></td><td><b> SetHead </b></td><td> Set the turtle Heading and Up vector. Params: 'hx, hy, hz, ux, uy, uz' or 'h,v' (optionals, default=0,0,1, 1,0,0).</td></tr>
	<tr><td><b>EulerAngles </b></td><td></td><td> Set the orientation of the turtle from the absolute euler angles. Params: 'azimuth, elevation, roll' (optionals, default=180,90,0).</td></tr>
	<tr><td><b>+ </b></td><td><b> Left </b></td><td> Turn left  around Up vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>- </b></td><td><b> Right </b></td><td> Turn right around Up vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>^ </b></td><td><b> Up </b></td><td> Pitch up around Left vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>& </b></td><td><b> Down </b></td><td> Pitch down around Left vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>/ </b></td><td><b> RollL </b></td><td> Roll left  around Heading vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>\ </b></td><td><b> RollR </b></td><td> Roll right  around Heading vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>iRollL </b></td><td></td><td> Roll left intrinsically around Heading vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>iRollR </b></td><td></td><td> Roll right intrinsically around Heading vector. Params : 'angle' (optional, in degrees).</td></tr>
	<tr><td><b>| </b></td><td><b> TurnAround </b></td><td> Turn around 180deg the Up vector.</td></tr>
	<tr><td><b>@v </b></td><td><b> RollToVert </b></td><td> Roll to Vertical : Roll the turtle around the H axis so that H and U lie in a common vertical plane with U closest to up</td></tr>
	<tr><td><b>LeftReflection </b></td><td></td><td> The turtle change the left vector to have a symmetric behavior.</td></tr>
	<tr><td><b>UpReflection </b></td><td></td><td> The turtle change the up vector to have a symmetric behavior.</td></tr>
	<tr><td><b>HeadingReflection </b></td><td></td><td> The turtle change the heading vector to have a symmetric behavior.</td></tr>
	<tr><td colspan=3 align="center"><H3>Position</H3></td></tr>
	<tr><td><b>@M </b></td><td><b> MoveTo </b></td><td> Set the turtle position. Params : 'x, y, z' or 'v' (optionals, default = 0).</td></tr>
	<tr><td><b>MoveRel </b></td><td></td><td> Move relatively from current the turtle position. Params : 'x, y, z' or 'v'(optionals, default = 0).</td></tr>
	<tr><td><b>@2D </b></td><td><b> StartScreenProjection </b></td><td> The turtle will create geometry in the screen coordinates system.</td></tr>
	<tr><td><b>@3D </b></td><td><b> EndScreenProjection </b></td><td> The turtle will create geometry in the world system (default behaviour).</td></tr>
	<tr><td colspan=3 align="center"><H3>Scale</H3></td></tr>
	<tr><td><b>@Dd </b></td><td><b> DivScale </b></td><td> Divides the current turtle scale by a scale factor, Params : 'scale_factor' (optional, default = 1.0).</td></tr>
	<tr><td><b>@Di </b></td><td><b> MultScale </b></td><td> Multiplies the current turtle scale by a scale factor, Params : 'scale_factor' (optional, default = 1.0).</td></tr>
	<tr><td><b>@D </b></td><td><b> SetScale </b></td><td> Set the current turtle scale, Params : 'scale' (optional, default = 1.0).</td></tr>
	<tr><td colspan=3 align="center"><H3>Primitive</H3></td></tr>
	<tr><td><b>F </b></td><td></td><td> Move forward and draw. Params: 'length , topradius'.</td></tr>
	<tr><td><b>f </b></td><td></td><td> Move forward and without draw. Params: 'length'.</td></tr>
	<tr><td><b>nF </b></td><td></td><td> Produce a n steps path of a given length and varying radius. Params : 'length, dlength [, radius = 1, radiusvariation = None]'.</td></tr>
	<tr><td><b>@Gc </b></td><td><b> StartGC </b></td><td> Start a new generalized cylinder.</td></tr>
	<tr><td><b>@Ge </b></td><td><b> EndGC </b></td><td> Pop generalized cylinder from the stack and render it.</td></tr>
	<tr><td><b>{ </b></td><td><b> BP </b></td><td> Start a new polygon.</td></tr>
	<tr><td><b>} </b></td><td><b> EP </b></td><td> Pop a polygon from the stack and render it. Params : concavetest (default=False).</td></tr>
	<tr><td><b>. </b></td><td><b> PP </b></td><td> Add a point for polygon.</td></tr>
	<tr><td><b>LineTo </b></td><td></td><td> Trace line to (x,y,z) without changing the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter' (optionals, default = 0).</td></tr>
	<tr><td><b>OLineTo </b></td><td></td><td> Trace line toward (x,y,z) and change the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter' (optionals, default = 0).</td></tr>
	<tr><td><b>LineRel </b></td><td></td><td> Trace line to pos+(x,y,z) without changing the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter'(optionals, default = 0).</td></tr>
	<tr><td><b>OLineRel </b></td><td></td><td> Trace line toward pos+(x,y,z) and change the orientation. Params : 'x, y, z, topdiameter' or 'v, topdiameter' (optionals, default = 0).</td></tr>
	<tr><td><b>@O </b></td><td><b> Sphere </b></td><td> Draw a sphere. Params : 'radius' (optional, should be positive, default = line width).</td></tr>
	<tr><td><b>@B </b></td><td><b> Box </b></td><td> Draw a box. Params : 'length','topradius'.</td></tr>
	<tr><td><b>@b </b></td><td><b> Quad </b></td><td> Draw a quad. Params : 'length','topradius'.</td></tr>
	<tr><td><b>@o </b></td><td><b> Circle </b></td><td> Draw a circle. Params : 'radius' (optional, should be positive, default = line width).</td></tr>
	<tr><td><b>@L </b></td><td><b> Label </b></td><td> Draw a text label. Params : 'text','size'.</td></tr>
	<tr><td><b>surface </b></td><td></td><td> Draw the predefined surface at the turtle's current location and orientation. Params : 'surface_name' (by default, 'l' exists), 'scale_factor' (optional, default= 1.0, should be positive).</td></tr>
	<tr><td><b>~ </b></td><td></td><td> Draw the predefined surface at the turtle's current location and orientation. Params : 'surface_name' (by default, 'l' exists), 'scale_factor' (optional, default= 1.0, should be positive).</td></tr>
	<tr><td><b>@g </b></td><td><b> PglShape </b></td><td> Draw a geometry at the turtle's current location and orientation. Params : 'geometric_model', 'scale_factor' (optional, should be positive) or 'shape' or 'scene' or 'material'.</td></tr>
	<tr><td><b>Frame </b></td><td></td><td> Draw the current turtle frame as 3 arrows (red=heading,blue=up,green=left). Params : 'size' (should be positive), 'cap_heigth_ratio' (in [0,1]), 'cap_radius_ratio' (should be positive).</td></tr>
	<tr><td><b>SetContour </b></td><td></td><td> Set Cross Section of Generalized Cylinder. Params : 'Curve2D [, ccw]'.</td></tr>
	<tr><td><b>SectionResolution </b></td><td></td><td> Set Resolution of Section of Cylinder. Params : 'resolution' (int).</td></tr>
	<tr><td><b>SetGuide </b></td><td></td><td> Set Guide for turtle tracing. Params : 'Curve[2D|3D], length [,yorientation, ccw]'.</td></tr>
	<tr><td><b>EndGuide </b></td><td></td><td> End Guide for turtle tracing.</td></tr>
	<tr><td><b>Sweep </b></td><td></td><td> Produce a sweep surface. Params : 'path, section, length, dlength [, radius = 1, radiusvariation = None]'.</td></tr>
	<tr><td><b>PositionOnGuide </b></td><td></td><td> Set position on Guide for turtle tracing.</td></tr>
	<tr><td colspan=3 align="center"><H3>Width</H3></td></tr>
	<tr><td><b>_ </b></td><td><b> IncWidth </b></td><td> Increase the current line width or set it if a parameter is given. Params : 'width' (optional).</td></tr>
	<tr><td><b>! </b></td><td><b> DecWidth </b></td><td> Decrease the current line width or set it if a parameter is given. Params : 'width' (optional).</td></tr>
	<tr><td><b>SetWidth </b></td><td></td><td> Set current line width. Params : 'width'.</td></tr>
	<tr><td colspan=3 align="center"><H3>Color</H3></td></tr>
	<tr><td><b>; </b></td><td><b> IncColor </b></td><td> Increase the current material index or set it if a parameter is given. Params : 'index' (optional, positive int).</td></tr>
	<tr><td><b>, </b></td><td><b> DecColor </b></td><td> Decrease the current material index or set it if a parameter is given. Params : 'index' (optional, positive int).</td></tr>
	<tr><td><b>SetColor </b></td><td></td><td> Set the current material. Params : 'index' (positive int) or 'r,g,b[,a]' or 'material'.</td></tr>
	<tr><td><b>InterpolateColors </b></td><td></td><td> Set the current material. Params : 'index1', 'index2', 'alpha' .</td></tr>
	<tr><td colspan=3 align="center"><H3>Tropism</H3></td></tr>
	<tr><td><b>@Ts </b></td><td><b> Elasticity </b></td><td> Set Branch Elasticity. Params : 'elasticity' (optional, default= 0.0, should be between [0,1]).</td></tr>
	<tr><td><b>@Tp </b></td><td><b> Tropism </b></td><td> Set Tropism. Params : 'tropism' (optional, Vector3, default= (1,0,0)).</td></tr>
	<tr><td colspan=3 align="center"><H3>Request</H3></td></tr>
	<tr><td><b>?P </b></td><td><b> GetPos </b></td><td> Request position vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).</td></tr>
	<tr><td><b>?H </b></td><td><b> GetHead </b></td><td> Request heading vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).</td></tr>
	<tr><td><b>?U </b></td><td><b> GetUp </b></td><td> Request up vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).</td></tr>
	<tr><td><b>?L </b></td><td><b> GetLeft </b></td><td> Request left vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).</td></tr>
	<tr><td><b>?R </b></td><td><b> GetRight </b></td><td> Request right vector information. Params : 'x,y,z' or 'v' (optional, default=Vector3, filled by Turtle).</td></tr>
	<tr><td><b>?F </b></td><td><b> GetFrame </b></td><td> Request turtle frame information. Params : 'p,h,u,l' (optional, filled by Turtle).</td></tr>
	<tr><td colspan=3 align="center"><H3>Texture</H3></td></tr>
	<tr><td><b>TextureScale </b></td><td></td><td> Set the scale coefficient for texture application. Params : 'uscale, vscale' (default = 1,1) or 'scale'.</td></tr>
	<tr><td><b>TextureUScale </b></td><td></td><td> Set the u-scale coefficient for texture application. Params : 'uscale' (default = 1).</td></tr>
	<tr><td><b>TextureVScale </b></td><td><b> TextureVCoeff </b></td><td> Set the v-scale coefficient for texture application. Params : 'vscale' (default = 1).</td></tr>
	<tr><td><b>TextureTranslation </b></td><td></td><td> Set the translation for texture application. Params : 'utranslation, vtranslation' (default = 0,0) or 'translation'.</td></tr>
	<tr><td><b>TextureRotation </b></td><td></td><td> Set the rotation for texture application. Params : 'angle, urotcenter, vrotcenter' (default = 0,0.5,0.5) or 'angle, rotcenter'.</td></tr>
	<tr><td><b>TextureTransformation </b></td><td></td><td> Set the transformation for texture application. Params : 'uscale, vscale, utranslation, vtranslation, angle, urotcenter, vrotcenter' (default = 1,1,0,0,0,0.5,0.5) or 'scale, translation, angle, rotcenter'.</td></tr>
	<tr><td colspan=3 align="center"><H3>String Manipulation</H3></td></tr>
	<tr><td><b>X </b></td><td><b> MouseIns </b></td><td> Module inserted just before module selected by user in visualisation.</td></tr>
	<tr><td><b>% </b></td><td><b> Cut </b></td><td> Cut the remainder of the current branch in the string.</td></tr>
	<tr><td><b>new </b></td><td><b> newmodule </b></td><td> Create a new module whose name is given by first argument.</td></tr>
	<tr><td colspan=3 align="center"><H3>Pattern Matching</H3></td></tr>
	<tr><td><b>=] </b></td><td></td><td> Match exactly a closing bracket</td></tr>
	<tr><td><b>* </b></td><td><b> any </b></td><td> Used to match any module in rules predecessor. First argument will become name of the module.</td></tr>
	<tr><td><b>x </b></td><td><b> repexp, all </b></td><td> Used to specify matching of a repetition of modules.</td></tr>
	<tr><td><b>or </b></td><td><b> || </b></td><td> Used to specify an alternative matching of modules.</td></tr>
	<tr><td><b>?I </b></td><td><b> GetIterator </b></td><td> Request an iterator over the current Lstring.</td></tr>
	<tr><td><b>$ </b></td><td><b> GetModule </b></td><td> Request a module of the current Lstring.</td></tr>
	</table>
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
	<li>  F. Boudon, T. Cokelaer, C. Pradal and C. Godin, L-Py, an open L-systems framework in Python, FSPM 2010.</li>
	<li>  P. Prusinkiewicz et al., 89, The algorithmic Beauty of Plants, Springer-Verlag.</li>
	<li>  P. Prusinkiewicz. Graphical applications of L-systems. Proceedings of Graphics Interface '86, pp. 247-253.</li>
	<li>  P. Prusinkiewicz, R. Karwowski, and B. Lane. The L+C plant modelling language. In Functional-Structural Plant Modelling in Crop Production, J. Vos et al. (eds.), Springer, 2007.</li>
	</ul>
