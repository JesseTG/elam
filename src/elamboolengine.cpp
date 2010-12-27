// ELAM int engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamboolengine.h"

#include<QDebug>

using namespace ELAM;

BoolEngine::BoolEngine()
{
	configureBoolEngine(*this);
	configureLogicEngine(*this);
}


static QVariant boolFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	if(!lf[0].canConvert<bool>())
		return Exception(Exception::TypeMismatchError,"cannot convert to bool");
	return lf[0].toBool();
}

//unary
static QVariant boolNot(const QVariant&o,Engine&)
{
	return !o.toBool();
}

//bitwise
static QVariant boolAnd(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toBool()&&o2.toBool();
}
static QVariant boolOr(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toBool()||o2.toBool();
}
static QVariant boolXor(const QVariant&o1,const QVariant&o2,Engine&)
{
	return o1.toBool()^o2.toBool();
}

QVariant boolCast(const QVariant&v,const Engine&){return v.toBool();}

void BoolEngine::configureBoolEngine(Engine&eng)
{
	int iid=QVariant::LongLong;
	int bid=QVariant::Bool;
	//cast
	eng.setFunction("bool",boolFunc);
	eng.setAutoCast(bid, QList<int>()<<QVariant::Int<<QVariant::UInt <<QVariant::LongLong<<QVariant::ULongLong <<QVariant::Char<<QVariant::String, boolCast, 20);
	//constants
	eng.setConstant("null",QVariant());
	eng.setConstant("true",true);
	eng.setConstant("false",false);
	//unaries
	eng.unaryOperator("!").setCallback(boolNot,iid);
	eng.unaryOperator("!").setCallback(boolNot,bid);
	eng.unaryOperator("~").setCallback(boolNot,bid);
	//binaries adapted
	eng.binaryOperator("&",50).setCallback(boolAnd,iid,bid);
	eng.binaryOperator("&",50).setCallback(boolAnd,bid,iid);
	eng.binaryOperator("&",50).setCallback(boolAnd,bid,bid);
	eng.binaryOperator("|",40).setCallback(boolOr,bid,iid);
	eng.binaryOperator("|",40).setCallback(boolOr,iid,bid);
	eng.binaryOperator("|",40).setCallback(boolOr,bid,bid);
	eng.binaryOperator("^",45).setCallback(boolXor,bid,iid);
	eng.binaryOperator("^",45).setCallback(boolXor,iid,bid);
	eng.binaryOperator("^",45).setCallback(boolXor,bid,bid);
	//booleans
	eng.binaryOperator("&&",30).setCallback(boolAnd,iid,iid);
	eng.binaryOperator("&&",30).setCallback(boolAnd,iid,bid);
	eng.binaryOperator("&&",30).setCallback(boolAnd,bid,iid);
	eng.binaryOperator("&&",30).setCallback(boolAnd,bid,bid);
	eng.binaryOperator("||",20).setCallback(boolOr,iid,iid);
	eng.binaryOperator("||",20).setCallback(boolOr,bid,iid);
	eng.binaryOperator("||",20).setCallback(boolOr,iid,bid);
	eng.binaryOperator("||",20).setCallback(boolOr,bid,bid);
	eng.binaryOperator("^^",25).setCallback(boolXor,iid,iid);
	eng.binaryOperator("^^",25).setCallback(boolXor,bid,iid);
	eng.binaryOperator("^^",25).setCallback(boolXor,iid,bid);
	eng.binaryOperator("^^",25).setCallback(boolXor,bid,bid);
}

static QVariant ifFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()<2 || lf.size()>3)
		return Exception(Exception::ArgumentListError, "expecting 2 or 3 arguments");
	if(!lf[0].canConvert<bool>())
		return Exception(Exception::TypeMismatchError,"cannot convert argument 1 to bool");
	if(lf[0].toBool())
		return lf[1];
	else
		if(lf.size()>2)return lf[2];
		return QVariant();
}

static QVariant isNullFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	return lf[0].isNull();
}

static QVariant isExceptionFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	return lf[0].userType()==Exception::metaTypeId();
}

static QVariant isExceptionOrNullFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	return lf[0].isNull() || lf[0].userType()==Exception::metaTypeId();
}

static QVariant catchFunc(const QList<QVariant>&lf,Engine&)
{
	if(lf.size()<1||lf.size()>3)
		return Exception(Exception::ArgumentListError, "expecting 1-3 arguments");
	if(lf[0].userType()==Exception::metaTypeId()){
		if(lf.size()>1)return lf[1];
		else return true;
	}else{
		if(lf.size()>2)return lf[2];
		else return false;
	}
}


void BoolEngine::configureLogicEngine(Engine& eng)
{
	eng.setFunction("if",ifFunc);
	eng.setFunction("isNull",isNullFunc);
	eng.setFunction("isException",isExceptionFunc);
	eng.setFunction("isExceptionOrNull",isExceptionOrNullFunc);
	eng.setFunction("catch",catchFunc);
}
