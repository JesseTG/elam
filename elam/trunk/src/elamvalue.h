//ELAM main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_VALUE_H
#define ELAM_VALUE_H

#include <QVariant>
#include <QString>

class ELAMValue
{
	public:
		ELAMValue();
		ELAMValue(long long);
		ELAMValue(double);
		ELAMValue(QString);
		ELAMValue(QVariant);
		
		
};

#endif
