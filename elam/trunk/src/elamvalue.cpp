//  value definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamvalue.h"

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

//end of namespace
};