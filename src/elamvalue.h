//ELAM value definition header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_VALUE_H
#define ELAM_VALUE_H

#include <QVariant>
#include <QString>
#include <QPair>

class ELAMValue
{
	public:
		ELAMValue();
		ELAMValue(long long);
		ELAMValue(double);
		ELAMValue(QString);
		ELAMValue(QVariant);
		ELAMValue& operator=(const ELAMValue&);
		ELAMValue& operator=(long long);
		ELAMValue& operator=(int);
		ELAMValue& operator=(const QString&);
		ELAMValue& operator=(const QVariant&);
		
		bool isNull()const{return m_val.isNull();}
		
		void setException(QString,int,int);
		
		bool hasException()const{return m_exc;}
		QString exceptionString()const{return m_exc?m_exstr:QString();}
		QPair<int,int> exceptionPos()const;
		
		QVariant toVariant()const{return m_val;}
		long long toInt()const{return m_val.toInt();}
		QString toString()const{return m_val.toString();}
		
	private:
		QVariant m_val;
		bool m_exc;
		QString m_exstr;
		int m_exline,m_excol;
};

#endif
