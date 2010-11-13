#include "../../src/dptr.h"

#include <QString>

class ClassWithDptr
{
	DECLARE_DPTR(d)
	public:
		QString toString()const;
		int num()const;
		void setNum(int);
};

class ClassWithSDptr
{
	DECLARE_SHARED_DPTR(d)
	public:
		QString toString()const;
		int num()const;
		void setNum(int);
};


#include <QObject>

class DPtrTest:public QObject
{
	Q_OBJECT
	private slots:
		void simpleDP();
		void sharedDP();
};
