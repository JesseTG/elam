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
// end of expression: anything not a nameClass.second
QPair<QString,QVariant> IntLiteralParser(const QString&expr,Engine&engine,int start)
{
	QString ls;
	//parse to end of expression
	QString nc=engine.characterClasses().nameClass().second;
	for(int i=start;i<expr.size();i++){
		if(nc.contains(expr[i]))ls+=expr[i];
		else break;
	}
	//is it integer?
	bool ok;
	qlonglong r=ls.toLongLong(&ok,0);
	if(ok)return QPair<QString,QVariant>(ls,r);
	else QPair<QString,QVariant>();
}

void IntEngine::configureIntEngine(ELAM::Engine& eng)
{
	//TODO: implement
	eng.setLiteralParser(IntLiteralParser,"0123456789",40);
}
