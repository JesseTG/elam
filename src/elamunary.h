// main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_ENGINE_UNARY_H
#define ELAM_ENGINE_UNARY_H

#include <QStringList>
#include <QObject>
#include <QVariant>

#include "dptr.h"

namespace ELAM {

/** \brief pointer to a function wrapping an unary operator
\param op the operand to be worked on
\returns the result of the operation*/
typedef QVariant (*UnaryOperatorCall)(const QVariant&op);

/** \brief Wraps a particular unary operator.

You can use the methods of this class to change the routines that handle the operator and the types on which it operates. Instances of this class are implicitly shared - meaning calls on a copy of an instance are also visible on the original and all other copies.
*/
class UnaryOperator
{
	DECLARE_SHARED_DPTR(d);
	public:
		/**copy constructor,
		the instance will access the exact same operator as the original, any setting that is done in the copy is also done in the original and all other copies.*/
		UnaryOperator(const UnaryOperator&);
		///instantiates an empty operator
		UnaryOperator();
		///deletes and operator
		~UnaryOperator();
		
		/**the operator becomes a shared copy of op and abandones its old link*/
		UnaryOperator& operator=(const UnaryOperator&op);
		
		/**sets a callback function for the operator and a specific typ
		\param callback the function to call, if it is null the type is deleted from this operators type list
		\param type the type of variable to work on, this must be a type registered with QVariant, if this type is already known to the operator its callback is replaced
		*/
		void setCallback(UnaryOperatorCall callback,QString type);
		/**sets a callback function for the operator and a specific typ
		\param callback the function to call, if it is null the type is deleted from this operators type list
		\param type the type of variable to work on, this must be a type registered with QVariant, if this type is already known to the operator its callback is replaced
		*/
		void setCallback(UnaryOperatorCall callback,int type);
		/**returns the callback function attached to the type or NULL if there is none*/
		UnaryOperatorCall getCallback(QString type)const;
		/**returns the callback function attached to the type or NULL if there is none*/
		UnaryOperatorCall getCallback(int type)const;
		
		/**removes all types attached to this callback from the operator*/
		void removeCallback(UnaryOperatorCall);
		///removes the type from this operators list
		void removeCallback(QString);
		///removes the type from this operators list
		void removeCallback(int);
		
		///returns all type names that have a valid callback in this operator
		QStringList getTypeNames()const;
		///returns all type IDs that have a valid callback in this operator
		QList<int> getTypeIds()const;
		
		///calls the callback function associated with the type of the argument, throws an exception if there is no callback
		QVariant execute(const QVariant&)const;
		
		///true if this operator has no callbacks
		bool isNull()const;
};

//end of namespace
};

#endif
