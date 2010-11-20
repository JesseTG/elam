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
	QString ex="a=b+=345*int(3.5)+ - -(+65/(5))";
	QVariant v=ie.evaluate(ex);
}


QTEST_MAIN(ElamTest)