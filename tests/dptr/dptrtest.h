#include "../../src/dptr.h"

#include <QString>

class ClassWithDptr
{
	DECLARE_DPTR(d)
	public:
		QString toString()const;
};

class ClassWithSDptr
{
	DECLARE_SHARED_DPTR(d)
	public:
		QString toString()const;
};


#include <QObject>

class DPtrTest:public QObject
{
	Q_OBJECT
	private slots:
		void simpleDP();
		void sharedDP();
};
