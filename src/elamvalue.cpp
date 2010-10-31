// ELAM value definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamvalue.h"

ELAMValue::ELAMValue()
{
	m_exc=false;
	m_exline=m_excol=-1;
}

ELAMValue::ELAMValue(long long l)
{
	m_exc=false;
	m_exline=m_excol=-1;
	m_val=l;
}

ELAMValue::ELAMValue(double d)
{
	m_exc=false;
	m_exline=m_excol=-1;
	m_val=d;
}

ELAMValue::ELAMValue(QString s)
{
	m_exc=false;
	m_exline=m_excol=-1;
	m_val=s;
}

ELAMValue::ELAMValue(QVariant v)
{
	m_exc=false;
	m_exline=m_excol=-1;
	//TODO: registered type?
	m_val=v;
}

ELAMValue& ELAMValue::operator=(const ELAMValue&e)
{
	m_exc=e.m_exc;
	m_exstr=e.m_exstr;
	m_exline=e.m_exline;
	m_excol=e.m_excol;
	m_val=e.m_val;
	return *this;
}

ELAMValue& ELAMValue::operator=(long long v)
{
	m_exc=false;m_excol=m_exline=-1;m_exstr=QString();
	m_val=v;
	return *this;
}

ELAMValue& ELAMValue::operator=(int v)
{
	m_exc=false;m_excol=m_exline=-1;m_exstr=QString();
	m_val=v;
	return *this;
}

ELAMValue& ELAMValue::operator=(const QString&v)
{
	m_exc=false;m_excol=m_exline=-1;m_exstr=QString();
	m_val=v;
	return *this;
}

ELAMValue& ELAMValue::operator=(const QVariant&v)
{
	m_exc=false;m_excol=m_exline=-1;m_exstr=QString();
	m_val=v;
	return *this;
}


void ELAMValue::setException(QString s,int l,int c)
{
	m_exc=true;
	m_exline=l;m_excol=c;
	m_exstr=s;
}

QPair<int,int> ELAMValue::exceptionPos()const
{
	if(m_exc)
		return QPair<int,int>(m_exline,m_excol);
	else
		return QPair<int,int>(-1,-1);
}
