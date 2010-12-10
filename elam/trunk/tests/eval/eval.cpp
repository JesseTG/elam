#include "elam.h"

#include "eval.h"

#include <QtCore>
#include <QtTest>
#include <QDebug>

using namespace ELAM;


void ElamTest::emptytest()
{
	QVariant v=Engine().evaluate("");
	QCOMPARE(v.userType(),Exception::metaTypeId());
}

void ElamTest::evaltest()
{
	IntEngine ie;
	FloatEngine::configureFloatEngine(ie);
	ie.setVariable("b",(qlonglong)0);
	QString exs="a=b+=345*int(3.15)+ - -(+65/(5))";
	Expression ex=ie.expression(exs);
// 	qDebug()<<"expression:\n"<<ex;
	QVariant v=ie.evaluate(ex);
	QVariant xv=Exception();
// 	if(v.userType()==Exception::metaTypeId())qDebug()<<"exception:"<<Exception(v);
	QVariant cmp=(qlonglong)(345*3+65/5);
	QCOMPARE(v,cmp);
	QCOMPARE(ie.getVariable("b"),cmp);
	QCOMPARE(ie.getVariable("a"),cmp);
	QCOMPARE(ie.evaluate("-2"),QVariant((qlonglong)-2));
	ie.setVariable("v",(qlonglong)2);
	ie.setConstant("c",(qlonglong)-3);
	QCOMPARE(ie.evaluate("-v"),QVariant((qlonglong)-2));
	QCOMPARE(ie.evaluate("-c"),QVariant((qlonglong)3));
	
}

static QVariant mycount(const QList<QVariant>&args)
{
	return (qlonglong)args.size();
}
static QVariant mysum(const QList<QVariant>&args)
{
	qlonglong r=0;
	for(int i=0;i<args.size();i++)
		r+=args[i].toLongLong();
	return r;
}

void ElamTest::excepttest()
{
	IntEngine ie;
	QVariant v=ie.evaluate("-novar");
	QCOMPARE(v.userType(),Exception::metaTypeId());
	Exception ex=v;
// 	qDebug()<<ex;
	QCOMPARE(ex.errorType(),Exception::UnknownValueError);
	ex=ie.evaluate("9/0");
	QCOMPARE(ex.errorType(),Exception::OperationError);
	ex=ie.evaluate("(novar)");
	QCOMPARE(ex.errorType(),Exception::UnknownValueError);
}

void ElamTest::counttest()
{
	IntEngine ie;
	ie.setFunction("count",mycount);
	QCOMPARE(ie.evaluate("count(1,2,3,count(4,5,6))"),QVariant((qlonglong)4));
}

void ElamTest::functest()
{
	IntEngine ie;
	ie.setFunction("count",mycount);
	ie.setFunction("sum",mysum);
	QCOMPARE(ie.evaluate("sum(1,2,3,count(1,x,n))"),QVariant((qlonglong)9));
}


QTEST_MAIN(ElamTest)