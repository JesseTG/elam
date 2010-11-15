// ELAM int engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamintengine.h"

#include<QDebug>

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
static QPair<QString,QVariant> IntLiteralParser(const QString&expr,Engine&engine,int start)
{
	QString ls;
	//parse to end of expression
	QString nc=engine.characterClasses().nameClass().second + "0123456789abcdefABCDEFx";
	for(int i=start;i<expr.size();i++){
		if(nc.contains(expr[i]))ls+=expr[i];
		else break;
	}
	//is it integer?
	bool ok;
	qlonglong r=ls.toLongLong(&ok,0);
	if(ok)return QPair<QString,QVariant>(ls,r);
	else return QPair<QString,QVariant>();
}

static QVariant intFunc(const QList<QVariant>&lf)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	if(!lf[0].canConvert<qlonglong>())
		return Exception(Exception::TypeMismatchError,"cannot convert to int");
	return lf[0].toLongLong();
}

static QVariant intPlus(const QVariant&o)
{
	return o;
}

static QVariant intMinus(const QVariant&o)
{
	return -o.toLongLong();
}

static QVariant intAdd(const QVariant&o1,const QVariant&o2)
{
	return o1.toLongLong()+o2.toLongLong();
}
static QVariant intMinus(const QVariant&o1,const QVariant&o2)
{
	return o1.toLongLong()-o2.toLongLong();
}
static QVariant intMult(const QVariant&o1,const QVariant&o2)
{
	return o1.toLongLong()*o2.toLongLong();
}
static QVariant intDiv(const QVariant&o1,const QVariant&o2)
{
	qlonglong l2=o2.toLongLong();
	if(l2==0)
		return Exception(Exception::OperationError,"division by zero");
	return o1.toLongLong()/l2;
}

int IntEngine::intParserPrio()
{
	return 20;
}


void IntEngine::configureIntEngine(ELAM::Engine& eng)
{
	int iid=QVariant::LongLong;
	eng.setLiteralParser(IntLiteralParser,"0123456789",intParserPrio());
	//cast
	eng.setFunction("int",intFunc);
	//unaries
	eng.unaryOperator("-").setCallback(intMinus,iid);
	eng.unaryOperator("+").setCallback(intPlus,iid);
	//binaries
	eng.binaryOperator("-").setCallback(intMinus,iid,iid);
	eng.binaryOperator("+").setCallback(intAdd,iid,iid);
	eng.binaryOperator("*").setCallback(intMult,iid,iid);
	eng.binaryOperator("/").setCallback(intDiv,iid,iid);
}
