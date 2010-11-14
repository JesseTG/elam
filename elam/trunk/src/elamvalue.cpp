//  value definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamvalue.h"

#include<QDebug>

namespace ELAM {
	
Exception::Exception()
{
}
Exception::Exception(const Exception& e)
{
	operator=(e);
}
Exception::Exception(ErrorType tp,QString errText, Position pos)
{
	mtype=tp;
	merr=errText;
	mpos=pos;
}

static int Exception_metaid=qRegisterMetaType<Exception>();
int Exception::metaTypeId()
{
	return Exception_metaid;
}
Exception& Exception::operator=(const Exception& e)
{
	merr=e.merr;
	mpos=e.mpos;
	return *this;
}

static int AnyType_metaid=qRegisterMetaType<AnyType>();
int AnyType::metaTypeId()
{
	return AnyType_metaid;
}

QDebug& operator<< ( QDebug& dbg, const ELAM::Position& pos)
{
	if(!pos.isValid())dbg.nospace()<<"Position(invalid)";
	else dbg.nospace()<<"Position(line="<<pos.line()<<",col="<<pos.column()<<")";
	return dbg.space();
}


//end of namespace
};