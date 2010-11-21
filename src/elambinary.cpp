//  engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamengine.h"
#include "elamvalue.h"

#include <QDebug>

namespace ELAM {
/////////////////////////////////////////////////////////////////
// Binary Operator

class DPTR_CLASS_NAME(BinaryOperator):public SharedDPtr
{
	public:
		QMap<QPair<int,int>,BinaryOperatorCall>callmap;
};
DEFINE_SHARED_DPTR(BinaryOperator)


BinaryOperator::BinaryOperator(const BinaryOperator& op)
	:d(op.d)
{
}

BinaryOperator& BinaryOperator::operator= ( const ELAM::BinaryOperator& op )
{
	d=op.d;
	return *this;
}


BinaryOperator::BinaryOperator()
{
}

BinaryOperator::~BinaryOperator()
{
}

QVariant BinaryOperator::execute ( const QVariant&op1,const QVariant&op2 ) const
{
	//search for match
	QPair<int,int>k(op1.userType(),op2.userType());
	QPair<int,int>kl(k.first,AnyType::metaTypeId());
	QPair<int,int>kr(AnyType::metaTypeId(),k.second);
	QPair<int,int>ka(AnyType::metaTypeId(),AnyType::metaTypeId());
	BinaryOperatorCall bc=0;
	//search for perfect, then left, then right, then any match
	if(d->callmap.contains(k))bc=d->callmap[k];else
	if(d->callmap.contains(kl))bc=d->callmap[kl];else
	if(d->callmap.contains(kr))bc=d->callmap[kr];else
	if(d->callmap.contains(ka))bc=d->callmap[ka];
	//bail out if none found
	if(bc==0)
		return Exception(Exception::TypeMismatchError, "operator cannot work on this type");
	//execute
	return bc(op1,op2);
}

BinaryOperatorCall BinaryOperator::getCallback ( QString type1, QString type2 ) const
{
	QPair<int,int>k( QVariant::nameToType(type1.toAscii().data()), QVariant::nameToType(type2.toAscii().data()));
	if(d->callmap.contains(k))
		return d->callmap[k];
	else
		return 0;
}

BinaryOperatorCall BinaryOperator::getCallback ( int type1, int type2 ) const
{
	QPair<int,int>k(type1,type2);
	if(d->callmap.contains(k))
		return d->callmap[k];
	else
		return 0;
}

QList< QPair< int, int > > BinaryOperator::getTypeIds() const
{
	return d->callmap.keys();
}
QList< QPair< QString, QString > > BinaryOperator::getTypeNames() const
{
	QList<QPair<int,int> > k=d->callmap.keys();
	QList<QPair<QString,QString> >ret;
	for(int i=0;i<k.size();i++)
		ret<<QPair<QString,QString>( QVariant::typeToName((QVariant::Type)k[i].first), QVariant::typeToName((QVariant::Type)k[i].second));
	return ret;
}
void BinaryOperator::removeCallback ( BinaryOperatorCall c)
{
	QList<QPair<int,int> > k=d->callmap.keys();
	for(int i=0;i<k.size();i++)
		if(d->callmap[k[i]]==c)
			d->callmap.remove(k[i]);
}
void BinaryOperator::removeCallback ( QString t1, QString t2)
{
	removeCallback( QVariant::nameToType(t1.toAscii().data()), QVariant::nameToType(t2.toAscii().data()));
}
void BinaryOperator::removeCallback ( int t1, int t2)
{
	d->callmap.remove(QPair<int,int>(t1,t2));
}
void BinaryOperator::setCallback ( BinaryOperatorCall callback, QString type1, QString type2 )
{
	setCallback(callback, QVariant::nameToType(type1.toAscii().data()), QVariant::nameToType(type2.toAscii().data()));
}
void BinaryOperator::setCallback ( BinaryOperatorCall callback, int type1, int type2 )
{
	if(callback==0)
		d->callmap.remove(QPair<int,int>(type1,type2));
	else
		d->callmap.insert(QPair<int,int>(type1,type2),callback);
}

bool BinaryOperator::isNull() const
{
	return d->callmap.isEmpty();
}


//end of namespace
};