/* ---------------------------------------------------------------------------
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


#include "patternmodule.h"
#include "patternstring.h"
#include "lsyscontext.h"
#include "lpy_parser.h"
#include "matching.h"

LPY_USING_NAMESPACE
using namespace boost::python;

/*---------------------------------------------------------------------------*/


LsysVar::LsysVar(const std::string& n):
	__name(n),__conditionType(NoCondition){}

LsysVar::LsysVar(boost::python::object value):
	__name(), __pyvalue(value), __conditionType(EqualValueCondition) {}


std::string LsysVar::str() const
{ 
	if (__conditionType == FunctionalCondition) {
		std::string res(__name);
		res += " if ";
		res += __textualcondition;
		return res;
	}
	else if(__conditionType == EqualValueCondition) {
		return extract<std::string>(bp::str(__pyvalue));
	}
	return __name; 
}


std::string LsysVar::varname() const
{ 
  if (__name.empty()) return __name;
  else if (__name.end() != __name.begin()+1 && __name[0] == '*' && __name[1] == '*') 
	  return std::string(__name.begin()+2,__name.end());
  else if (__name[0] == '*') return std::string(__name.begin()+1,__name.end());
  else return __name;
}


bool LsysVar::isCompatible(const boost::python::object& value) const
{
	switch(__conditionType) {
		case EqualValueCondition:
			return value == __pyvalue;
		case FunctionalCondition:
			return bp::call<bool>(__pyvalue.ptr(),value);
		default:
			return true;
	}
}

void LsysVar::setCondition(const std::string& textualcondition, int lineno)
{
	std::string txt = "lambda "+varname()+" : "+textualcondition;
	if (lineno > 0){
		std::string decal ;
		for(size_t i = 0; i < lineno-1; ++i)
			decal += '\n';
		txt = decal + txt;
	}
	__pyvalue = LsysContext::current()->evaluate(txt);
	__textualcondition = textualcondition;
	__conditionType = FunctionalCondition;
}

void LsysVar::setUnnamed()
{
  if (__name.empty()) __name = "-";
  else if (__name.end() != __name.begin()+1 && __name[0] == '*' && __name[1] == '*') __name = "**-";
  else if (__name[0] == '*') __name = "*-";
  else __name = "-";
}

/*---------------------------------------------------------------------------*/
PatternModule::PatternModule(): BaseType() { }

PatternModule::PatternModule(const std::string& name, int lineno): BaseType()
{
  std::vector<std::pair<size_t,std::string> > parsedstring = LpyParsing::parselstring(name,lineno);
  if(parsedstring.size() != 1)LsysError("Invalid query module "+name,"",lineno);
  setClass(parsedstring[0].first);
  __processPatternModule(parsedstring[0].second,lineno);
}

PatternModule::PatternModule(size_t classid, const std::string& argstr, int lineno):
	BaseType(classid) {
  __processPatternModule(argstr,lineno);
}

PatternModule::~PatternModule() { }



std::vector<std::string> PatternModule::getVarNames() const
{
  std::vector<std::string> res;
  if (isRepExp()) {
	extract<const PatternString&> t(getAt(0).getPyValue());
	if(t.check()) return t().getVarNames();
  }
  else if (isOr()) {
	  for(size_t i = 0; i < size(); i++){
		  extract<const PatternString&> v(getAt(i).getPyValue());
		  if(v.check()) {
		   std::vector<std::string> lres = v().getVarNames();
		   res.insert(res.end(),lres.begin(),lres.end());
		  }
	  }
  }
  else if (isGetModule()) {
    // var should be in one
	res.push_back(getAt(0).varname());
	/*extract<const PatternModule&> v(getAt(0).getPyValue());
	if(v.check()) {
	   std::vector<std::string> lres = v().getVarNames();
	   res.insert(res.end(),lres.begin(),lres.end());
	}*/
  }
  else {
	  for(size_t i = 0; i < size(); i++){
		  if(getAt(i).isNamed()) res.push_back(getAt(i).varname());
	  }
  }
  return res;
}

size_t PatternModule::getVarNb() const
{
  size_t res = 0;
  if (isRepExp()) {
	extract<const PatternString&> t(getAt(0).getPyValue());
	if(t.check()) return t().getVarNb();
  }
  else if (isOr()) {
	  for(size_t i = 0; i < size(); i++){
		  extract<const PatternString&> v(getAt(i).getPyValue());
		  if(v.check()) res += v().getVarNb();
	  }
  }
  else if (isGetModule()) {
    // suppose to have one var in first position
	if(getAt(0).isNamed()) res += 1;
	/* extract<const PatternModule&> v(getAt(1).getPyValue());
	if(v.check()) res += v().getVarNb();*/
  }
  else {
	  for(size_t i = 0; i < size(); i++){
		  if(getAt(i).isNamed()) res+=1;
	  }
  }
  return res;
}

#define check_var_order(var,itarg,endarg,shouldBeArgs) \
	if(var.isKwds()) { \
		if (itarg == endarg-1) ; \
		else { LsysError("Invalid syntax : invalid variable name '"+*itarg+"'","",lineno); } \
	} \
	else if(var.isArgs()) { \
		if (itarg == endarg-1) ; \
        else if (itarg == args.end()-2) shouldBeArgs = true; \
		else { LsysError("Invalid syntax : invalid variable name '"+*itarg+"'","",lineno); } \
	} \
	else if (shouldBeArgs) { \
		LsysError("Invalid syntax : invalid variable name '"+getAt(size()-1).str()+"'","",lineno); \
	} \


void PatternModule::__processPatternModule(const std::string& argstr, int lineno)
{
  if (getClass() == ModuleClass::RepExp) {
	  std::vector<std::string> args = LpyParsing::parse_arguments(argstr);
	  if (args.empty())LsysError("No Matching Pattern in RepExp module","",lineno);
	  else if (args.size() > 3) LsysError("Too much parameters in RepExp module","",lineno);
	  append(LsysVar(boost::python::object(PatternString(args[0],lineno))));
	  if (args.size() > 1) append(LsysVar(LsysContext::currentContext()->evaluate(args[1])));
	  if (args.size() == 3) append(LsysVar(LsysContext::currentContext()->evaluate(args[2])));
  }
  else if (getClass() == ModuleClass::Or) {
	  std::vector<std::string> args = LpyParsing::parse_arguments(argstr);
	  if (args.size() < 2)LsysError("Not enough parameters in Or module","",lineno);
	  for(size_t i = 0; i < args.size(); ++i){
		append(LsysVar(boost::python::object(PatternString(args[i],lineno))));
	  }
  }
  else if (getClass() == ModuleClass::GetModule) {
	  // get all arguments. suppose to have 2 : one var and one class
	  std::vector<std::string> args = LpyParsing::parse_arguments(argstr);
	  if (args.size() != 2)LsysError("Invalid construction of $.","",lineno);
	  // we look for a var name in 0 and a module class in 1 
	  std::pair<std::string,std::string> vartxt = LpyParsing::parse_variable(args[0],lineno);
	  // add var
	  if(LpyParsing::isValidVariableName(vartxt.first)){
		LsysVar var(vartxt.first);
		if(!vartxt.second.empty())var.setCondition(vartxt.second,lineno);
		append(var);
	  }
	  else LsysError("Invalid construction of $. No variable set.","",lineno);
	  // add class module
	  PatternModule p(args[1],lineno);
	  if (p.size() != 0)LsysError("Invalid construction of $.","",lineno);
	  append(LsysVar(boost::python::object(p.getClass())));
  }
  else {
	  std::vector<std::string> args = LpyParsing::parse_arguments(argstr);
	  bool shouldBeArgs = false;
	  bool error = false;
	  std::string msgerror;

	  for(std::vector<std::string>::const_iterator itarg = args.begin(); itarg != args.end(); ++itarg){
		  bool notvar = true;
		  if (MatchingEngine::getModuleMatchingMethod() == MatchingEngine::eMWithStarNValueConstraint){
			 std::pair<std::string,std::string> vartxt;
			 try {
				 vartxt = LpyParsing::parse_variable(*itarg,lineno);				 
				 notvar = false;
			  }
			  catch (boost::python::error_already_set) {   PyErr_Clear(); /* PyErr_Print();*/ }
			  if (!notvar){
				if(vartxt.first == "-"){
					append(LsysVar(vartxt.first));
				}
				else if(LpyParsing::isValidVariableName(vartxt.first)){
					LsysVar var(vartxt.first);
				    if(!vartxt.second.empty()) var.setCondition(vartxt.second,lineno);
					check_var_order(var,itarg,args.end(),shouldBeArgs)
				    append(var);
				}
			  }
			  else {
			      object o = LsysContext::currentContext()->try_evaluate(*itarg);
			      if(o != object()){ append(o); notvar = true; }
			  }
		  }
		  else {
			  if(*itarg == "-" || LpyParsing::isValidVariableName(*itarg)){
				  LsysVar var(*itarg);
				  check_var_order(var,itarg,args.end(),shouldBeArgs);
				  append(var);
			  }
			  else LsysError(*itarg+" is invalid","",lineno);
		  }
		  // var order checking

	  }
  }
}

void PatternModule::setUnnamedVariables()
{
  if (isRepExp()) {
	extract<PatternString&> t(getAt(0).getPyValue());
	if(t.check()) t().setUnnamedVariables();
  }
  else if (isOr()) {
	  for(size_t i = 0; i < size(); i++){
		  extract<PatternString&> v(getAt(i).getPyValue());
		  if(v.check()) v().setUnnamedVariables();
	  }
  }
  else if (isGetModule()) {
    // unamed first var
	if(getAt(0).isNamed()) getAt(0).setUnnamed();
	// extract<PatternModule&> v(getAt(1).getPyValue());
	// if(v.check()) v().setUnnamedVariables();
  }
  else {
	  for(size_t i = 0; i < size(); i++){
		  if(getAt(i).isNamed()) getAt(i).setUnnamed();
	  }
  }
}

void PatternModule::setUnnamedVariable(size_t idvar)
{
  size_t count = 0;
  if (isRepExp()) {
	extract<PatternString&> t(getAt(0).getPyValue());
	if(t.check()) t().setUnnamedVariable(idvar);
  }
  else if (isOr()) {
	  for(size_t i = 0; i < size(); i++){
		  extract<PatternString&> v(getAt(i).getPyValue());
		  if(v.check()) {
			  PatternString& ps = v();
			  size_t l = ps.getVarNb();
			  if(count + l > idvar){
				v().setUnnamedVariable(idvar - count);
				break;
			  }
			  else count += l;
		  }
	  }
  }
  else if (isGetModule()) {
	  // unamed the var if necessary
	  if(getAt(0).isNamed()) {
		  count += 1;
		  if(idvar == 0) getAt(0).setUnnamed();
	  }
	  // extract<PatternModule&> v(getAt(1).getPyValue());
	  // if(v.check()) {
	  //	 v().setUnnamedVariable(idvar - count);
	  //}
  }
  else {
	  for(size_t i = 0; i < size(); i++){
		  if(getAt(i).isNamed()) { 
			  count += 1;
			  if(count == idvar+1) getAt(i).setUnnamed();
		  }
	  }
  }
}

std::vector<size_t> PatternModule::getFirstClassId() const
{ return getBorderClassId(eForward); }


std::vector<size_t> PatternModule::getLastClassId() const
{ return getBorderClassId(eBackward); }

std::vector<size_t> PatternModule::getBorderClassId(eDirection dir) const
{
	std::vector<size_t> res;
    if (isRepExp()) {
	   extract<PatternString&> t(getAt(0).getPyValue());
	   if(t.check()) {
		   std::vector<size_t> lres = (dir == eForward?t().getFirstClassId():t().getLastClassId());
		   res.insert(res.end(),lres.begin(),lres.end());
	   }
  }
  else if (isOr()) {
	  for(size_t i = 0; i < size(); i++){
		  extract<PatternString&> v(getAt(i).getPyValue());
		  if(v.check()) {
		    std::vector<size_t> lres = (dir == eForward?v().getFirstClassId():v().getLastClassId());
		    res.insert(res.end(),lres.begin(),lres.end());
		  }
	  }
  }
  else if (isGetModule()) {
	  extract<PatternModule&> v(getAt(1).getPyValue());
	  if(v.check()) {
		  res.push_back(v().getClass()->getId());
	  }
	  else res.push_back(ModuleClass::Star->getId());
  }
  else {
	  res.push_back(getClass()->getId());
  }
  return res;
}
/*---------------------------------------------------------------------------*/

