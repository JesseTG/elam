//  value definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamvalue.h"

#include<QDebug>

namespace ELAM {
	
Exception::Exception(const Exception& e)
{
	operator=(e);
}

Exception::Exception(const QVariant& v)
{
	operator=(v.value<Exception>());
}
Exception::Exception(Exception::ErrorType type, Position pos, QString errorText)
{
	mtype=type;
	merr=errorText;
	mpos=pos;
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
	mtype=e.mtype;
	merr=e.merr;
	mpos=e.mpos;
	return *this;
}

static int AnyType_metaid=qRegisterMetaType<ELAM::AnyType>();
int AnyType::metaTypeId()
{
	return AnyType_metaid;
}

QDebug& operator<< ( QDebug dbg, const ELAM::Position& pos)
{
	if(!pos.isValid())dbg.nospace()<<"Position(invalid)";
	else dbg.nospace()<<"Position(line="<<pos.line()<<",col="<<pos.column()<<")";
	return dbg.space();
}

QDebug&operator<<(QDebug dbg,const Exception&ex)
{
	dbg.nospace()<<"Exception(type=";
	switch(ex.errorType()){
		case Exception::NoError:dbg<<"NoError";break;
		case Exception::ParserError:dbg<<"ParserError";break;
		case Exception::UnknownOperatorError:dbg<<"UnknownOperation";break;
		case Exception::UnknownFunctionError:dbg<<"UnknownFunction";break;
		case Exception::UnknownValueError:dbg<<"UnknownValue";break;
		case Exception::TypeMismatchError:dbg<<"TypeMismatch";break;
		case Exception::ArgumentListError:dbg<<"ArgumentList";break;
		case Exception::OperationError:dbg<<"Operation";break;
		default:dbg<<"Unknown:"<<(int)ex.errorType();break;
	}
	if(ex.errorText()!="")
		dbg<<",text="<<ex.errorText();
	dbg<<",pos="<<ex.errorPos();
	dbg.nospace()<<")";
	return dbg.space();
}
QDebug&operator<<(QDebug dbg,const AnyType&){dbg.nospace()<<"AnyType";return dbg.space();}


//end of namespace
};