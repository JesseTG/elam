// main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_ENGINE_BINARY_H
#define ELAM_ENGINE_BINARY_H

#include <QStringList>
#include <QObject>
#include <QVariant>

#include "dptr.h"

namespace ELAM {

/** \brief pointer to a function wrapping a binary operator
\param op1 the left operand
\param op2 the right operand
\returns the result of the operation*/
typedef QVariant (*BinaryOperatorCall)(const QVariant&op1,const QVariant&op2);

/** \brief Wraps a particular binary operator.

You can use the methods of this class to change the routines that handle the operator and the types on which it operates. Instances of this class are implicitly shared - meaning calls on a copy of an instance are also visible on the original and all other copies.
*/
class BinaryOperator
{
	DECLARE_SHARED_DPTR(d);
	public:
		BinaryOperator(const BinaryOperator&);
		BinaryOperator();
		~BinaryOperator();
		/**the operator becomes a shared copy of op and abandones its old link*/
		BinaryOperator& operator=(const BinaryOperator&op);
		
		/**sets a callback function for the operator and a specific typ
		\param callback the function to call, if it is null the type is deleted from this operators type list
		\param type the type of variable to work on, this must be a type registered with QVariant, if this type is already known to the operator its callback is replaced
		*/
		void setCallback(BinaryOperatorCall callback,QString type1,QString type2);
		/**sets a callback function for the operator and a specific typ
		\param callback the function to call, if it is null the type is deleted from this operators type list
		\param type the type of variable to work on, this must be a type registered with QVariant, if this type is already known to the operator its callback is replaced
		*/
		void setCallback(BinaryOperatorCall callback,int type1,int type2);
		/**returns the callback function attached to the type or NULL if there is none*/
		BinaryOperatorCall getCallback(QString type1,QString type2)const;
		/**returns the callback function attached to the type or NULL if there is none*/
		BinaryOperatorCall getCallback(int type1,int type2)const;
		
		/**removes all types attached to this callback from the operator*/
		void removeCallback(BinaryOperatorCall);
		///removes the type from this operators list
		void removeCallback(QString,QString);
		///removes the type from this operators list
		void removeCallback(int,int);
		
		///returns all combinations of type names that have a valid callback in this operator
		QList<QPair<QString,QString> > getTypeNames()const;
		///returns all combinations of type IDs that have a valid callback in this operator
		QList<QPair<int,int> > getTypeIds()const;
		
		///calls the callback function associated with the type of the argument, throws an exception if there is no callback
		QVariant execute(const QVariant&,const QVariant&)const;
		
		///true if this operator has no callbacks
		bool isNull()const;
};

//end of namespace
};

#endif
