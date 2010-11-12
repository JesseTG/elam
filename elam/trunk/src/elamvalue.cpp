// ELAM value definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamvalue.h"

ELAMException::ELAMException()
{
	mline=mcol=-1;
}
ELAMException::ELAMException(const ELAMException& e)
{
	operator=(e);
}
ELAMException::ELAMException(ErrorType tp,QString errText, int line, int column)
{
	mtype=tp;
	merr=errText;
	mline=line;
	mcol=column;
}
static int ELAMException_metaid=qRegisterMetaType<ELAMException>();
int ELAMException::metaTypeId()
{
	return ELAMException_metaid;
}
ELAMException& ELAMException::operator=(const ELAMException& e)
{
	merr=e.merr;
	mline=e.mline;
	mcol=e.mcol;
	return *this;
}

static int ELAMAnyType_metaid=qRegisterMetaType<ELAMAnyType>();
int ELAMAnyType::metaTypeId()
{
	return ELAMAnyType_metaid;
}
