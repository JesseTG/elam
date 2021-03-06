// ELAM int engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamfloatengine.h"
#include "elamintengine.h"

#include <QDebug>

using namespace ELAM;

FloatEngine::FloatEngine()
{
	configureFloatEngine(*this);
}

IntFloatEngine::IntFloatEngine()
{
	IntEngine::configureIntEngine(*this);
	FloatEngine::configureFloatEngine(*this);
}


/* There are two literal parser routines because of possible interactions with int:
 
 floatLiteralParser1 has higher priority than the default int parser and tries to assess whether this is a real float (ie. it contains a dot or exponent)
 
 floatLiteralParser2 has lower priority than the default int parser and acts as a fallback in case no int parser is installed, it consumes the remaining floats that would normally be ints
*/

//primary float parser: tries to find a dot or ...e...
static QPair<QString,QVariant> floatLiteralParser1(const QString&expr,Engine&engine,int start)
{
	QString ls;
	//parse to end of expression
	QString nc=engine.characterClasses().nameClass().second + "0123456789.eE-+";
	for(int i=start;i<expr.size();i++){
		if(nc.contains(expr[i]))ls+=expr[i];
		else break;
	}
	ls=ls.toLower();
	if(!ls.contains('.') && !ls.contains('e'))
		return QPair<QString,QVariant>();
	//is it float?
	bool ok;
	qreal r=ls.toDouble(&ok);
	if(ok)return QPair<QString,QVariant>(ls,r);
	else return QPair<QString,QVariant>();
}

//secondary float parser: fallback for int
static QPair<QString,QVariant> floatLiteralParser2(const QString&expr,Engine&engine,int start)
{
	QString ls;
	//parse to end of expression
	QString nc=engine.characterClasses().nameClass().second + "0123456789.eE-+";
	for(int i=start;i<expr.size();i++){
		if(nc.contains(expr[i]))ls+=expr[i];
		else break;
	}
	//is it float?
	bool ok;
	qreal r=ls.toDouble(&ok);
	if(ok)return QPair<QString,QVariant>(ls,r);
	else return QPair<QString,QVariant>();
}

static QVariant floatAdd(const QVariant &o1,const QVariant &o2,Engine&)
{
	return o1.toDouble()+o2.toDouble();
}
static QVariant floatPlus(const QVariant &o,Engine&)
{
	return o;
}
static QVariant floatMinus(const QVariant &o,Engine&)
{
	return -o.toDouble();
}
static QVariant floatMinus(const QVariant &o1,const QVariant &o2,Engine&)
{
	return o1.toDouble()-o2.toDouble();
}
static QVariant floatMult(const QVariant &o1,const QVariant &o2,Engine&)
{
	return o1.toDouble()*o2.toDouble();
}
static QVariant floatDiv(const QVariant &o1,const QVariant &o2,Engine&)
{
	double d2=o2.toDouble();
	if(d2==0.0)
		return QVariant::fromValue<Exception>(Exception(Exception::OperationError,"division by zero"));
	return o1.toDouble()/d2;
}

static QVariant fltLt(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toDouble()<o2.toDouble();
}
static QVariant fltGt(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toDouble()>o2.toDouble();
}
static QVariant fltLe(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toDouble()<=o2.toDouble();
}
static QVariant fltGe(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toDouble()>=o2.toDouble();
}
static QVariant fltEq(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toDouble()==o2.toDouble();
}
static QVariant fltNe(const QVariant&o1,const QVariant&o2,Engine&)
{
        return o1.toDouble()!=o2.toDouble();
}


static QVariant floatFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	if(!lf[0].canConvert<qreal>())
		return Exception(Exception::TypeMismatchError,"cannot convert to float");
	return lf[0].toDouble();
}

int FloatEngine::floatHighParserPrio()
{
	return 30;
}
int FloatEngine::floatLowParserPrio()
{
	return 10;
}

static QVariant fltCast(const QVariant&v,const Engine&){return v.toDouble();}

void FloatEngine::configureFloatEngine(ELAM::Engine& eng)
{
	eng.setLiteralParser(floatLiteralParser1,"0123456789.",floatHighParserPrio());
	eng.setLiteralParser(floatLiteralParser2,"0123456789.",floatLowParserPrio());
	int fid=QVariant::Double;
	int iid=QVariant::LongLong;
	eng.setAutoCast(fid, QList<int>()<<QVariant::Int<<QVariant::LongLong<<QVariant::UInt<<QVariant::ULongLong, fltCast, 30);
	//unary
	eng.unaryOperator("-").setCallback(floatMinus,fid);
	eng.unaryOperator("+").setCallback(floatPlus,fid);
	//binary
        eng.binaryOperator("==",60).setCallback(fltEq,fid,fid);
        eng.binaryOperator("==").setCallback(fltEq,fid,iid);
        eng.binaryOperator("==").setCallback(fltEq,iid,fid);
        eng.binaryOperator("!=",60).setCallback(fltNe,fid,fid);
        eng.binaryOperator("!=").setCallback(fltNe,iid,fid);
        eng.binaryOperator("!=").setCallback(fltNe,fid,iid);
        eng.binaryOperator("<=",60).setCallback(fltLe,fid,fid);
        eng.binaryOperator("<=").setCallback(fltLe,fid,iid);
        eng.binaryOperator("<=").setCallback(fltLe,iid,fid);
        eng.binaryOperator(">=",60).setCallback(fltGe,fid,fid);
        eng.binaryOperator(">=").setCallback(fltGe,iid,fid);
        eng.binaryOperator(">=").setCallback(fltGe,fid,iid);
        eng.binaryOperator("<",60).setCallback(fltLt,fid,fid);
        eng.binaryOperator("<").setCallback(fltLt,iid,fid);
        eng.binaryOperator("<").setCallback(fltLt,fid,iid);
        eng.binaryOperator(">",60).setCallback(fltGt,fid,fid);
        eng.binaryOperator(">").setCallback(fltGt,iid,fid);
        eng.binaryOperator(">").setCallback(fltGt,fid,iid);
	eng.binaryOperator("-",80).setCallback(floatMinus,fid,fid);
	eng.binaryOperator("-").setCallback(floatMinus,iid,fid);
	eng.binaryOperator("-").setCallback(floatMinus,fid,iid);
	eng.binaryOperator("+",80).setCallback(floatAdd,fid,fid);
	eng.binaryOperator("+").setCallback(floatAdd,iid,fid);
	eng.binaryOperator("+").setCallback(floatAdd,fid,iid);
	eng.binaryOperator("*",90).setCallback(floatMult,fid,fid);
	eng.binaryOperator("*").setCallback(floatMult,iid,fid);
	eng.binaryOperator("*").setCallback(floatMult,fid,iid);
	eng.binaryOperator("/",90).setCallback(floatDiv,fid,fid);
	eng.binaryOperator("/").setCallback(floatDiv,iid,fid);
	eng.binaryOperator("/").setCallback(floatDiv,fid,iid);
	//cast
	eng.setFunction("float",floatFunc);
}
