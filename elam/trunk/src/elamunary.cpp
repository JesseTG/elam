//  engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamengine.h"
#include "elamvalue.h"

#include <QDebug>

namespace ELAM {

/////////////////////////////////////////////////////////////////
// Unary Operator

class DPTR_CLASS_NAME(UnaryOperator):public SharedDPtr
{
	public:
		QMap<int,UnaryOperatorCall>callmap;
};
DEFINE_SHARED_DPTR(UnaryOperator)


UnaryOperator::UnaryOperator(const UnaryOperator& op)
	:d(op.d)
{
}

UnaryOperator& UnaryOperator::UnaryOperator::operator=(const ELAM::UnaryOperator& op)
{
	d=op.d;
	return *this;
}


UnaryOperator::UnaryOperator()
{
}

UnaryOperator::~UnaryOperator()
{
}


QVariant UnaryOperator::execute(const QVariant& op) const
{
	if(d->callmap.size()==0)
		return qVariantFromValue<Exception>(Exception(Exception::UnknownOperatorError));
	//search for direct match
	if(d->callmap.contains(op.type()))
		return d->callmap[op.type()](op);
	//search for fallback
	int any=AnyType::metaTypeId();
	if(d->callmap.contains(any))
		return d->callmap[any](op);
	return qVariantFromValue<Exception>(Exception(Exception::TypeMismatchError));
}

QList< int > UnaryOperator::getTypeIds() const
{
	return d->callmap.keys();
}

QStringList UnaryOperator::getTypeNames() const
{
	QStringList ret;
	QList<int>ti=d->callmap.keys();
	for(int i=0;i<ti.size();i++)
		ret<<QVariant::typeToName((QVariant::Type)ti[i]);
	return ret;
}

UnaryOperatorCall UnaryOperator::getCallback(QString type) const
{
	return getCallback(QVariant::nameToType(type.toAscii().data()));
}

UnaryOperatorCall UnaryOperator::getCallback(int type) const
{
	if(d->callmap.contains(type))
		return d->callmap[type];
	if(d->callmap.contains(AnyType::metaTypeId()))
		return d->callmap[AnyType::metaTypeId()];
	return 0;
}

void UnaryOperator::setCallback(UnaryOperatorCall callback, int type)
{
	if(type==QVariant::Invalid)return;
	if(callback==0){
		d->callmap.remove(type);
		return;
	}
	d->callmap.insert(type,callback);
}

void UnaryOperator::setCallback(UnaryOperatorCall callback, QString type)
{
	setCallback(callback,QVariant::nameToType(type.toAscii().data()));
}

void UnaryOperator::removeCallback(UnaryOperatorCall cb)
{
	if(cb==0)return;
	QList<int>k=d->callmap.keys(cb);
	for(int i=0;i<k.size();i++)
		d->callmap.remove(k[i]);
}

void UnaryOperator::removeCallback(QString t)
{
	removeCallback(QVariant::nameToType(t.toAscii().data()));
}

void UnaryOperator::removeCallback(int t)
{
	d->callmap.remove(t);
}


//end of namespace
};