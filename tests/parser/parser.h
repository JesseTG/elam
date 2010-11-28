#include <QObject>

class ElamTest:public QObject
{
	Q_OBJECT
	private slots:
		void charClass();
		void tokenizer();
		void stringLiteral();
};
