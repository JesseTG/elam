
#include "dptrtest.h"

#include <QtCore>
#include <QtTest>
#include <QDebug>

void DPtrTest::simpleDP()
{
	qDebug()<<"dynamic"<<ClassWithDptr().toString();
	qDebug()<<"dynamic"<<ClassWithDptr().toString();
	ClassWithDptr so;
	qDebug()<<"static"<<so.toString();
	ClassWithDptr co(so);
	qDebug()<<"static copy"<<co.toString();
	qDebug()<<"dynamic copy"<<ClassWithDptr(so).toString();
	co=ClassWithDptr();
	qDebug()<<"overwritten"<<co.toString();
}

void DPtrTest::sharedDP()
{
	qDebug()<<"dynamic"<<ClassWithSDptr().toString();
	qDebug()<<"dynamic"<<ClassWithSDptr().toString();
	ClassWithSDptr so;
	qDebug()<<"static"<<so.toString();
	ClassWithSDptr co(so);
	qDebug()<<"static copy"<<co.toString();
	qDebug()<<"dynamic copy"<<ClassWithSDptr(so).toString();
	co=ClassWithSDptr();
	qDebug()<<"overwritten"<<co.toString();
	so=co;
	qDebug()<<"overwritten"<<so.toString();
}


QTEST_MAIN(DPtrTest)