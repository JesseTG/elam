
#include "dptrtest.h"

#include <QtCore>

class DPTR_CLASS_NAME(ClassWithDptr):public DPtr
{
	public:
		int num;
		Private(){num=0;}
};
DEFINE_DPTR(ClassWithDptr)


QString ClassWithDptr::toString()const
{
	return QString("class with dptr %1").arg(d->num);
}
int ClassWithDptr::num()const{return d->num;}
void ClassWithDptr::setNum(int n){d->num=n;}

class DPTR_CLASS_NAME(ClassWithSDptr):public SharedDPtr
{
	public:
		int num;
		Private(){num=0;}
};
DEFINE_SHARED_DPTR(ClassWithSDptr)

QString ClassWithSDptr::toString()const
{
	return QString("class with shared dptr %1").arg(d->num);
}
int ClassWithSDptr::num()const{return d->num;}
void ClassWithSDptr::setNum(int n){d->num=n;}
