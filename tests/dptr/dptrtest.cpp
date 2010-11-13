
#include "dptrtest.h"

#include <QtCore>
#include <QtTest>
#include <QDebug>

void DPtrTest::simpleDP()
{
	QCOMPARE(ClassWithDptr().num(),0);
	ClassWithDptr o1;o1.setNum(1);
	QCOMPARE(o1.num(),1);
	ClassWithDptr o2;o2.setNum(2);
	ClassWithDptr o3(o1);
	QCOMPARE(o1.num(),1);
	QCOMPARE(o2.num(),2);
	QCOMPARE(o3.num(),1);
	o1=o2;
	QCOMPARE(o1.num(),2);
	o2.setNum(3);
	QCOMPARE(o1.num(),2);
	QCOMPARE(o2.num(),3);
}

void DPtrTest::sharedDP()
{
	QCOMPARE(ClassWithSDptr().num(),0);
	ClassWithSDptr o1;o1.setNum(1);
	QCOMPARE(o1.num(),1);
	ClassWithSDptr o2;o2.setNum(2);
	ClassWithSDptr o3(o1);
	QCOMPARE(o1.num(),1);
	QCOMPARE(o2.num(),2);
	QCOMPARE(o3.num(),1);
	o1.setNum(4);
	QCOMPARE(o1.num(),4);
	QCOMPARE(o2.num(),2);
	QCOMPARE(o3.num(),4);
	o2=o1;o2.setNum(5);
	QCOMPARE(o1.num(),5);
	QCOMPARE(o2.num(),5);
	QCOMPARE(o3.num(),5);
}


QTEST_MAIN(DPtrTest)