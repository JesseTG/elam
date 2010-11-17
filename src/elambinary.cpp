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
	//TODO: implement
	return QVariant();
}

BinaryOperatorCall BinaryOperator::getCallback ( QString type1, QString type2 ) const
{
	//TODO: implement
	return 0;
}

BinaryOperatorCall BinaryOperator::getCallback ( int type1, int type2 ) const
{
	//TODO: implement
	return 0;
}

QList< QPair< int, int > > BinaryOperator::getTypeIds() const
{
	//TODO: implement
	return QList< QPair< int, int > > ();
}
QList< QPair< QString, QString > > BinaryOperator::getTypeNames() const
{
	//TODO: implement
	return QList< QPair< QString, QString > >();
}
void BinaryOperator::removeCallback ( BinaryOperatorCall )
{
	//TODO: implement

}
void BinaryOperator::removeCallback ( QString , QString )
{
	//TODO: implement

}
void BinaryOperator::removeCallback ( int , int )
{
	//TODO: implement

}
void BinaryOperator::setCallback ( BinaryOperatorCall callback, QString type1, QString type2 )
{
	//TODO: implement

}
void BinaryOperator::setCallback ( BinaryOperatorCall callback, int type1, int type2 )
{
	//TODO: implement

}

//end of namespace
};