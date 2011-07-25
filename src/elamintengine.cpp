// ELAM floating point engine definition implementation
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

static QVariant intFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	if(!lf[0].canConvert<qlonglong>())
		return Exception(Exception::TypeMismatchError,"cannot convert to int");
	return lf[0].toLongLong();
}

//unary
static QVariant intPlus(const QVariant&o,Engine&)
{
	return o;
}
static QVariant intMinus(const QVariant&o,Engine&)
{
	return -o.toLongLong();
}

//additive
static QVariant intAdd(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toLongLong()+o2.toLongLong();
}
static QVariant intMinus(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toLongLong()-o2.toLongLong();
}

//multiplicative
static QVariant intMult(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toLongLong()*o2.toLongLong();
}
static QVariant intDiv(const QVariant&o1,const QVariant&o2,Engine&)
{
	qlonglong l2=o2.toLongLong();
	if(l2==0)
		return Exception(Exception::OperationError,"division by zero");
	return o1.toLongLong()/l2;
}
static QVariant intMod(const QVariant&o1,const QVariant&o2,Engine&)
{
	qlonglong l2=o2.toLongLong();
	if(l2==0)
		return Exception(Exception::OperationError,"division by zero");
	return o1.toLongLong()%l2;
}

//bitwise
static QVariant intAnd(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toLongLong()&o2.toLongLong();
}
static QVariant intOr(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toLongLong()|o2.toLongLong();
}
static QVariant intXor(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toLongLong()^o2.toLongLong();
}
static QVariant intNot(const QVariant&o,Engine&)
{
	return ~o.toLongLong();
}

static QVariant intLt(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toLongLong()<o2.toLongLong();
}
static QVariant intGt(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toLongLong()>o2.toLongLong();
}
static QVariant intLe(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toLongLong()<=o2.toLongLong();
}
static QVariant intGe(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toLongLong()>=o2.toLongLong();
}
static QVariant intEq(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toLongLong()==o2.toLongLong();
}
static QVariant intNe(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toLongLong()!=o2.toLongLong();
}


int IntEngine::intParserPrio()
{
	return 20;
}

static QVariant intCast(const QVariant&v,const Engine&){return v.toLongLong();}

void IntEngine::configureIntEngine(ELAM::Engine& eng)
{
	int iid=QVariant::LongLong;
	eng.setLiteralParser(IntLiteralParser,"0123456789",intParserPrio());
	eng.setAutoCast(iid, QList<int>()<<QVariant::Int<<QVariant::UInt<<QVariant::ULongLong, intCast, 40);
	eng.setAutoCast(iid, QList<int>()<<QVariant::Double, intCast, 30);
	//cast
	eng.setFunction("int",intFunc);
	//unaries
	eng.unaryOperator("-").setCallback(intMinus,iid);
	eng.unaryOperator("+").setCallback(intPlus,iid);
	eng.unaryOperator("~").setCallback(intNot,iid);
	//binaries
        eng.binaryOperator("==",60).setCallback(intEq,iid,iid);
        eng.binaryOperator("!=",60).setCallback(intNe,iid,iid);
        eng.binaryOperator("<=",60).setCallback(intLe,iid,iid);
        eng.binaryOperator(">=",60).setCallback(intGe,iid,iid);
        eng.binaryOperator("<",60).setCallback(intLt,iid,iid);
        eng.binaryOperator(">",60).setCallback(intGt,iid,iid);
	eng.binaryOperator("-",80).setCallback(intMinus,iid,iid);
	eng.binaryOperator("+",80).setCallback(intAdd,iid,iid);
	eng.binaryOperator("*",90).setCallback(intMult,iid,iid);
	eng.binaryOperator("/",90).setCallback(intDiv,iid,iid);
	eng.binaryOperator("%",90).setCallback(intMod,iid,iid);
	eng.binaryOperator("&",50).setCallback(intAnd,iid,iid);
	eng.binaryOperator("|",40).setCallback(intOr,iid,iid);
	eng.binaryOperator("^",45).setCallback(intXor,iid,iid);
}
