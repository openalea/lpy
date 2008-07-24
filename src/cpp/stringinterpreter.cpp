/* 
# ---------------------------------------------------------------------------
#
#       L-Py: L-systems in Python
#
#       Copyright 2003-2008 UMR Cirad/Inria/Inra Dap - Virtual Plant Team
#
#       File author(s): F. Boudon (frederic.boudon@cirad.fr)
#
# ---------------------------------------------------------------------------
#
#                      GNU General Public Licence
#
#       This program is free software; you can redistribute it and/or
#       modify it under the terms of the GNU General Public License as
#       published by the Free Software Foundation; either version 2 of
#       the License, or (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
#       GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public
#       License along with this program; see the file COPYING. If not,
#       write to the Free Software Foundation, Inc., 59
#       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# ---------------------------------------------------------------------------
*/

#include "mod.h"
#include "axialtree.h"
#include "stringinterpreter.h"
#include "lsyscontext.h"
#include "tracker.h"
#include <sstream>

PGL_USING_NAMESPACE
TOOLS_USING_NAMESPACE
PYLSYS_USING_NAMESPACE

StringInterpreter::StringInterpreter(Turtle* turtle):
__turtle(turtle){
  initCommand();
  IncTracker(StringInterpreter)
}

StringInterpreter::~StringInterpreter() { DecTracker(StringInterpreter) }

const Turtle&
StringInterpreter::getTurtle() const { return *__turtle; }

Turtle&
StringInterpreter::getTurtle() { return *__turtle; }

void StringInterpreter::setTurtle(PGL::Turtle* turtle) { __turtle = turtle; }

void StringInterpreter::initCommand(){
  addCommand("[","Push the state in the stack.");
  addCommand("]","Pop a state from the stack and make it the current state of the turtle.");
  addCommand("f","Move forward");
  addCommand("F","Move forward and Draw. Params: F(length,topradius).");
  addCommand("g","Move forward");
  addCommand("G","Move forward and Draw");
  addCommand("+","Turn left  around Up vector");
  addCommand("-","Turn right around Up vector");
  addCommand("&","Pitch down around Left vector");
  addCommand("^","Pitch up   around Left vector");
  addCommand("/","Roll left  around Heading vector");
  addCommand("\\","Roll right around Heading vector");
  addCommand("|","Turn around 180 around Up vector");
  //addCommand("#","Increase the current line width or set it if a parameter is given");
  addCommand("!","Decrease the current line width or set it if a parameter is given");
  addCommand(";","Increase the current material index or set it if a parameter is given");
  addCommand(",","Decrease the current material index or set it if a parameter is given");
  addCommand("@M","Set the turtle position");
  addCommand("@R","Set the turtle Heading vector (optionnaly Up vector too)");
  addCommand("@v","Roll to Vertical : Roll the turtle around the H axis so that H and U lie in a common vertical plane with U closest to up");
  addCommand("@O","Draw a sphere of diameter d");
  addCommand("@o","Draw a circle of diameter d in the HL plane");
  addCommand("@c","Draw a circle of diameter d in the HL plane");
  addCommand("~","Draw the predefined surface identified by the symbol immediately following the ~ at the turtle's current location and orientation.");
  addCommand("{","Start a new polygon");
  addCommand("}","Pop a polygon from the stack and render it");
  addCommand("@Gs","Start a new generalized cylinder");
  addCommand("@Ge","Pop a generalized cylinder from the stack and render it");
  addCommand("@L","Draw label");
  addCommand("@D","Sets the current turtle scale");
  addCommand("@Di","Multiplies the current turtle scale");
  addCommand("@Dd","Divides the current turtle scale");
  addCommand("?P","Request position vector information");
  addCommand("?H","Request heading vector information");
  addCommand("?U","Request up vector information");
  addCommand("?L","Request left vector information");

  addCommand("_","* Increase the current line width or set it if a parameter is given. Replace #");
  addCommand("@g","* Draw a custom geometry");
  addCommand("?R","* Request right vector information");
}

void StringInterpreter::addCommand(const std::string& command,
								   const std::string& comment){
  __comments.push_back(std::pair<std::string,std::string>(command,comment));
}


std::string StringInterpreter::help(const std::string& command){
    std::stringstream stream;
  if(command.empty()){
	for (std::vector<std::pair<std::string,std::string> >::const_iterator i =__comments.begin();
	i != __comments.end(); i++)
	  stream << i->first << " : "<< i->second << std::endl;
  }
  else{
	std::vector<std::pair<std::string,std::string> >::const_iterator i =__comments.begin();
	for (;i != __comments.end() && i->first != command ;)i++;
	if (i !=  __comments.end())
	  stream << i->first << " : "<< i->second << std::endl;
  }
  return stream.str();
}

void StringInterpreter::printHelp(const std::string& command)
{
    std::cout << help(command);
}

/*
void StringInterpreter::plot(AxialTree& tree){
  interpret(tree);
  PGLViewerApplication::display(turtle.getScene()());
}*/

void StringInterpreter::interpret(AxialTree& tree){
  __turtle->start();
  __turtle->setId(0);
  for(AxialTree::iterator _it = tree.begin();
      _it != tree.end(); ++_it) {
        interpret(*_it);
        if(__turtle->getId() != Shape::NOID)__turtle->incId();
  }
  __turtle->stop();
  if (!__turtle->emptyStack()){
      printf("Turtle stack size : %i\n",__turtle->getStack().size());
	LsysError("Ill-formed string: unmatched brackets");
  }
}

void StringInterpreter::interpret(AxialTree& tree, const StringMatching& matching ){
  __turtle->start();
  StringMatching::const_iterator _iditer = matching.begin();
  __turtle->setId(0);
  for(AxialTree::iterator _it = tree.begin();_it != tree.end(); ++_it,++_iditer){
        interpret(*_it);
        if(__turtle->getId() != Shape::NOID)__turtle->setId(*_iditer);
  }
  __turtle->stop();
  if (!__turtle->emptyStack()){
      printf("Turtle stack size : %i\n",__turtle->getStack().size());
	LsysError("Ill-formed string: unmatched brackets");
  }
}

void StringInterpreter::interpret(ParamModule& mod){
	mod.interpret(*__turtle);
    /*PglTurtle * pglturtle = NULL;
    Turtle& turtle = *__turtle;
    bool turtle_test = false;
	
    if(!mod.name().empty()){
        char n = mod.name()[0]; 
        switch (n) {
            case '[':
                {
                    turtle.push();
                }
                break;
            case ']':
                {
                    turtle.pop();
                }
                break;
            case 'g':
            case 'f':
                {
                    if (!mod.hasArg())turtle.f();
                    else   turtle.f(mod._getReal(0));
                }
                break;
            case 'G':
            case 'F':
                {
                    if (!mod.hasArg())turtle.F();
                    else if (mod.argSize() == 1)turtle.F(mod._getReal(0));
                    else   turtle.F(mod._getReal(0),mod._getReal(1));
                }
                break;
            case '+':
                if (!mod.hasArg())turtle.left();
                else   turtle.left(mod._getReal(0));
                break;
            case '-':
                if (!mod.hasArg())turtle.right();
                else   turtle.right(mod._getReal(0));
                break;
            case '&':
                if (!mod.hasArg())turtle.down();
                else   turtle.down(mod._getReal(0));
                break;
            case '^':
                if (!mod.hasArg())turtle.up();
                else   turtle.up(mod._getReal(0));
                break;
            case '/':
                if (!mod.hasArg())turtle.rollR();
                else   turtle.rollR(mod._getReal(0));
                break;
            case '\\':
                if (!mod.hasArg())turtle.rollL();
                else   turtle.rollL(mod._getReal(0));
                break;
            case '|':
                turtle.left(180);
                break;
            case '_':
            case '#':
                if (!mod.hasArg())turtle.incWidth();
                else   turtle.setWidth(mod._getReal(0));
                break;
            case '!':
                if (!mod.hasArg())turtle.decWidth();
                else   turtle.setWidth(mod._getReal(0));
                break;
            case ';':
                if (!mod.hasArg())turtle.incColor();
                else   turtle.setColor(mod._getInt(0));
                break;
            case ',':
                if (!mod.hasArg())turtle.decColor();
                else   turtle.setColor(mod._getInt(0));
                break;
            case '~':
                // if (mod.name().size() > 1){
                //    std::string name(mod.name().begin()+1, mod.name().end());
				if (!mod.hasArg())LsysWarning("No symbol name found for module ~");
                else if ( mod.argSize() == 1 )turtle.surface(mod._getString(0));
                else turtle.surface(mod._getString(0),mod._getReal(1));
                break;
            case '{':
                turtle.startPolygon();
                break;
            case '}':
                turtle.stopPolygon();
                break;
            case '?':
                if(mod.name().size() > 1){
                    Vector3 w;
                    bool ok = true;
                    switch (mod.name()[1]){
                        case 'P':
                            w = turtle.getPosition();
                            break;
                        case 'H':
                            w = turtle.getHeading();
                            break;
                        case 'U':
                            w = turtle.getUp();
                            break;
                        case 'L':
                            w = turtle.getLeft();
                            break;
                        case 'R':
                            w = -turtle.getLeft();
                            break;
                        default:
                            ok = false;
                            break;
                    }
                    if (ok)mod._setValues(w.x(),w.y(),w.z());
                }
                break;
            case '@':
                if(mod.name().size() > 1){
                    switch (mod.name()[1]){
                       case 'L':
                           if (mod.hasArg())turtle.label(mod._getString(0));
                           break;
                       case 'T':
                           if(mod.name().size() > 1 && mod.name()[2] == 'x'){
                               if (!mod.hasArg())turtle.setTexture(0);
                               else   turtle.setTexture(mod._getInt(0));
                           }
                           break;
                       case 'G':
                           if(mod.name().size() > 2){
                               switch (mod.name()[2]){
                                    case 's':
                                        turtle.startGC();
                                    break;
                                    case 'e':
                                        turtle.stopGC();
                                    break;
                                    default:
                                    break;
                               }
                           }
                           break;				  
                       case 'D':
                           if(mod.name().size() > 2){
                               switch (mod.name()[2]){
                                    case 'i':
                                        if (!mod.hasArg())turtle.multScale();
                                        else   turtle.multScale(mod._getReal(0));
                                    break;
                                    case 'd':
                                        if (!mod.hasArg())turtle.divScale();
                                        else   turtle.divScale(mod._getReal(0));
                                        break;
                                    default:
                                        if (!mod.hasArg())turtle.scale();
                                        else   turtle.scale(mod._getReal(0));
                                        break;
                                    }
                                }
                           else {
                                if (!mod.hasArg())turtle.scale();
                                else   turtle.scale(mod._getReal(0));
                            }
                           break;				  
                       case 'M':
                           switch (mod.argSize()){
                                case 0: 
                                    turtle.move(); 
                                    break;
                                case 1: 
                                    turtle.move(mod._getReal(0)); 
                                    break;
                                case 2: 
                                    turtle.move(mod._getReal(0),mod._getReal(1)); 
                                    break;
                                default: 
                                    turtle.move(mod._getReal(0),mod._getReal(1),mod._getReal(2)); 
                                    break;
                           }
                           break;
                       case 'R':
                           switch (mod.argSize()){
                                case 0: turtle.setHead(); break;
                                case 1: turtle.setHead(mod._getReal(0)); break;
                                case 2: turtle.setHead(mod._getReal(0),mod._getReal(1)); break;
                                case 3: turtle.setHead(mod._getReal(0),mod._getReal(1),mod._getReal(2)); break;
                                case 4: turtle.setHead(mod._getReal(0),mod._getReal(1),mod._getReal(2),
                                            mod._getReal(3)); break;
                                case 5: turtle.setHead(mod._getReal(0),mod._getReal(1),mod._getReal(2),
                                            mod._getReal(3),mod._getReal(4)); break;
                                default: turtle.setHead(mod._getReal(0),mod._getReal(1),mod._getReal(2),
                                             mod._getReal(3),mod._getReal(4),mod._getReal(5)); break;
                           }
                           break;
                       case 'v':
                           turtle.rollToVert();
                           break;
                       case 'O':
                           if (!mod.hasArg())turtle.sphere();
                           else   turtle.sphere(mod._getReal(0));
                           break;
                       case 'c':
                       case 'o':
                           if (!mod.hasArg())turtle.circle();
                           else   turtle.circle(mod._getReal(0));
                           break;
                       case 'g':
                           if (!turtle_test){
                             pglturtle = dynamic_cast<PglTurtle *>(__turtle);
                             turtle_test = true;
                           }
                           if (pglturtle){
                            if (!mod.hasArg())LsysWarning("No geometry found for module @g");
                            else   pglturtle->customGeometry(boost::python::extract<GeometryPtr>(mod.getAt(0)));
                           }
                           break;
                       default:
                           break;
                    }
                }
            default:
                ;
        }
    }*/
}

/*---------------------------------------------------------------------------*/

std::string PYLSYS::helpTurtle(const std::string& command){
  Turtle t;
  StringInterpreter i(&t);
  return i.help(command);
}

void PYLSYS::printHelpTurtle(const std::string& command){
  Turtle t;
  StringInterpreter i(&t);
  return i.printHelp(command);
}
/*---------------------------------------------------------------------------*/

std::string PYLSYS::helpTurtle() { return helpTurtle(""); }

void PYLSYS::printHelpTurtle() { return printHelpTurtle(""); }

/*---------------------------------------------------------------------------*/

void PYLSYS::interpret(AxialTree& tree){
  Turtle t;
  interpret(tree,t);
}

void PYLSYS::interpret(AxialTree& tree, Turtle& turtle){
  StringInterpreter s(&turtle);
  s.interpret(tree);
}

/*---------------------------------------------------------------------------*/


