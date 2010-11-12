
#include "dptrtest.h"

#include <QtCore>

class DPTR_CLASS_NAME(ClassWithDptr)
{
	public:
		static int cnt;
		int num;
		Private(){num=cnt++;qDebug()<<"creates dptr"<<num;}
		Private(const Private&p){num=p.num;qDebug()<<"copyconst dptr"<<num;}
		~Private(){qDebug()<<"delete dptr"<<num;}
		Private& operator=(const Private&p){
			qDebug()<<"copy dptr"<<p.num<<"onto"<<num;
			num=p.num;
			return *this;
		}
};
int ClassWithDptr::Private::cnt=0;
DEFINE_DPTR(ClassWithDptr)


QString ClassWithDptr::toString()const
{
	return QString("class with dptr %1").arg(d->num);
}

class DPTR_CLASS_NAME(ClassWithSDptr):public SharedDPtr
{
	public:
		static int cnt;
		int num;
		Private(){num=cnt++;qDebug()<<"creates dptr"<<num;}
		Private(const Private&p){num=p.num;qDebug()<<"copyconst dptr"<<num;}
		~Private(){qDebug()<<"delete dptr"<<num;}
		Private& operator=(const Private&p){
			qDebug()<<"copy dptr"<<p.num<<"onto"<<num;
			num=p.num;
			return *this;
		}
};
int ClassWithSDptr::Private::cnt=0;
DEFINE_SHARED_DPTR(ClassWithSDptr)

QString ClassWithSDptr::toString()const
{
	return QString("class with shared dptr %1").arg(d->num);
}
