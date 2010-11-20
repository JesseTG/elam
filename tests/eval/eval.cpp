#include "elam.h"

#include "eval.h"

#include <QtCore>
#include <QtTest>
#include <QDebug>

using namespace ELAM;


void ElamTest::evaltest()
{
	IntEngine ie;
	FloatEngine::configureFloatEngine(ie);
	QString exs="a=b+=345*int(3.5)+ - -(+65/(5))";
	Expression ex=ie.expression(exs);
	qDebug()<<"expression:"<<ex;
	QVariant v=ie.evaluate(ex);
}


QTEST_MAIN(ElamTest)