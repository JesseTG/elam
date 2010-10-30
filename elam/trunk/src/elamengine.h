//ELAM main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_ENGINE_H
#define ELAM_ENGINE_H

#include <QString>
#include <QObject>

#include "elamexpression.h"

class ELAMEngine:public QObject
{
	Q_OBJECT
	public:
		ELAMEngine(QObject*parent=0);
	public slots:
		ELAMExpression parseExpression(QString);
		ELAMValue evaluate(QString);
		ELAMValue evaluate(ELAMExpression);
	private:
		class Private;
		Private *d;
};

#endif
