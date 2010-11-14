// ELAM int engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamintengine.h"

using namespace ELAM;

IntEngine::IntEngine()
{
	configureIntEngine(*this);
}

//types of int
// decimal: [1-9][0-9]*
// octal: 0[0-7]*
// hex: 0x[0-9a-fA-F]+
QPair<QString,QVariant> IntLiteralParser(const QString&expr,Engine&engine,int start)
{
	QString ls;
	//shortcut: single char?
	if(start==(expr.size()-1)){
		ls=expr.mid(start,1);
		return QPair<QString,QVariant>(ls,ls.toInt());
	}
	//check type
	if(expr[start]>'0' && expr[start]<='9'){
		//decimal
		for(int i=start;i<expr.size();i++){
			if(expr[i]<'0' || expr[i]>'9')break;
			ls+=expr[i];
		}
	}else //oct or hex
	if(expr[start+1]=='x'){
		//hex
		ls="0x";
		for(int i=start+2;i<expr.size();i++){
			if((expr[i]>='0' && expr[i]<='9')&&
			   (expr[i]>='a' && expr[i]<='f')&&
			   (expr[i]>='A' && expr[i]<='F'))
				ls+=expr[i];
			else
				break;
		}
	}else{
		//oct
		for(int i=start;i<expr.size();i++){
			if(expr[i]<'0' || expr[i]>'7')break;
			ls+=expr[i];
		}
	}
	return QPair<QString,QVariant>(ls,ls.toLongLong(0,0));
}

void IntEngine::configureIntEngine(ELAM::Engine& eng)
{
	//TODO: implement
	eng.setLiteralParser(IntLiteralParser,"0123456789",40);
}
