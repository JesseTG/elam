// ELAM engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamengine.h"
#include "elamvalue.h"

class DPTR_CLASS_NAME(ELAMEngine)
{
	
};

DEFINE_DPTR(ELAMEngine);

ELAMEngine::ELAMEngine(QObject* parent): QObject(parent)
{

}

QVariant ELAMEngine::evaluate(QString ex)
{
	return evaluate(parseExpression(ex));
}

QVariant ELAMEngine::evaluate(ELAMExpression ex)
{
	//TODO: implement
	return QVariant();
}

ELAMExpression ELAMEngine::parseExpression(QString )
{
	//TODO: implement parser
	return ELAMExpression();
}

/////////////////////////////////////////////////////////////////
// Unary Operator

class DPTR_CLASS_NAME(ELAMUnaryOperator):public SharedDPtr
{
	public:
		QMap<int,ELAMUnaryOperatorCall>callmap;
};
DEFINE_SHARED_DPTR(ELAMUnaryOperator)


ELAMUnaryOperator::ELAMUnaryOperator(const ELAMUnaryOperator& op)
	:d(op.d)
{
}

ELAMUnaryOperator::ELAMUnaryOperator()
{
}

ELAMUnaryOperator::~ELAMUnaryOperator()
{
}


QVariant ELAMUnaryOperator::execute(const QVariant& op) const
{
	if(d->callmap.size()==0)
		return qVariantFromValue<ELAMException>(ELAMException(ELAMException::UnknownOperatorError));
	//search for direct match
	if(d->callmap.contains(op.type()))
		return d->callmap[op.type()](op);
	//search for fallback
	int any=ELAMAnyType::metaTypeId();
	if(d->callmap.contains(any))
		return d->callmap[any](op);
	return qVariantFromValue<ELAMException>(ELAMException(ELAMException::TypeMismatchError));
}

QList< int > ELAMUnaryOperator::getTypeIds() const
{
	return d->callmap.keys();
}

QStringList ELAMUnaryOperator::getTypeNames() const
{
	QStringList ret;
	QList<int>ti=d->callmap.keys();
	for(int i=0;i<ti.size();i++)
		ret<<QVariant::typeToName((QVariant::Type)ti[i]);
	return ret;
}

ELAMUnaryOperatorCall ELAMUnaryOperator::getCallback(QString type) const
{
	return getCallback(QVariant::nameToType(type.toAscii().data()));
}

ELAMUnaryOperatorCall ELAMUnaryOperator::getCallback(int type) const
{
	if(d->callmap.contains(type))
		return d->callmap[type];
	if(d->callmap.contains(ELAMAnyType::metaTypeId()))
		return d->callmap[ELAMAnyType::metaTypeId()];
	return 0;
}

void ELAMUnaryOperator::setCallback(ELAMUnaryOperatorCall callback, int type)
{
	if(type==QVariant::Invalid)return;
	if(callback==0){
		d->callmap.remove(type);
		return;
	}
	d->callmap.insert(type,callback);
}

void ELAMUnaryOperator::setCallback(ELAMUnaryOperatorCall callback, QString type)
{
	setCallback(callback,QVariant::nameToType(type.toAscii().data()));
}

void ELAMUnaryOperator::removeCallback(ELAMUnaryOperatorCall cb)
{
	if(cb==0)return;
	QList<int>k=d->callmap.keys(cb);
	for(int i=0;i<k.size();i++)
		d->callmap.remove(k[i]);
}

void ELAMUnaryOperator::removeCallback(QString t)
{
	removeCallback(QVariant::nameToType(t.toAscii().data()));
}

void ELAMUnaryOperator::removeCallback(int t)
{
	d->callmap.remove(t);
}

/////////////////////////////////////////////////////////////////
// Unary Operator

class DPTR_CLASS_NAME(ELAMBinaryOperator):public SharedDPtr
{
	public:
		QMap<QPair<int,int>,ELAMBinaryOperatorCall>callmap;
};
DEFINE_SHARED_DPTR(ELAMBinaryOperator)


ELAMBinaryOperator::ELAMBinaryOperator(const ELAMBinaryOperator& op)
	:d(op.d)
{
}

ELAMBinaryOperator::ELAMBinaryOperator()
{

}

ELAMBinaryOperator::~ELAMBinaryOperator()
{

}
