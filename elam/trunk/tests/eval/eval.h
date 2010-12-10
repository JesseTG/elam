#include <QObject>

class ElamTest:public QObject
{
	Q_OBJECT
	private slots:
		void emptytest();
		void evaltest();
		void excepttest();
		void counttest();
		void functest();
};
