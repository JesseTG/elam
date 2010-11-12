//ELAM main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_ENGINE_H
#define ELAM_ENGINE_H

#include <QStringList>
#include <QObject>
#include <QVariant>

#include "elamexpression.h"

/**pointer to a function wrapping an unary operator
\param op the operand to be worked on
\returns the result of the operation*/
typedef QVariant (*ELAMUnaryOperatorCall)(const QVariant&op);

class ELAMUnaryOperator
{
	public:
		ELAMUnaryOperator(const ELAMUnaryOperator&);
		ELAMUnaryOperator();
		~ELAMUnaryOperator();
		
		ELAMUnaryOperator& operator=(const ELAMUnaryOperator&);

		/**sets a callback function for the operator and a specific typ
		\param callback the function to call, if it is null the type is deleted from this operators type list
		\param type the type of variable to work on, this must be a type registered with QVariant, if this type is already known to the operator its callback is replaced
		*/
		void setCallback(ELAMUnaryOperatorCall callback,QString type);
		/**sets a callback function for the operator and a specific typ
		\param callback the function to call, if it is null the type is deleted from this operators type list
		\param type the type of variable to work on, this must be a type registered with QVariant, if this type is already known to the operator its callback is replaced
		*/
		void setCallback(ELAMUnaryOperatorCall callback,int type);
		/**returns the callback function attached to the type or NULL if there is none*/
		ELAMUnaryOperatorCall getCallback(QString type)const;
		/**returns the callback function attached to the type or NULL if there is none*/
		ELAMUnaryOperatorCall getCallback(int type)const;
		
		/**removes all types attached to this callback from the operator*/
		void removeCallback(ELAMUnaryOperatorCall);
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
	private:
		class Private;
		Private *d;
};

/**pointer to a function wrapping a binary operator
\param op1 the left operand
\param op2 the right operand
\returns the result of the operation*/
typedef QVariant (*ELAMBinaryOperatorCall)(const QVariant&op1,const QVariant&op2);

/**pointer to a function wrapping a mathematical function
\param args the list of arguments
\returns the result of the function*/
typedef QVariant (*ELAMFunction)(const QList<QVariant>&args);


/**The calculation engine of ELAM.

Instances of this class can be configured to represent a specific system of functions and operators.

Methods of this class are used to evaluate expressions into final values.
*/
class ELAMEngine:public QObject
{
	Q_OBJECT
	public:
		///instantiates an engine object
		ELAMEngine(QObject*parent=0);
		
	public slots:
		///simply parses an expression string into an ELAM object
		ELAMExpression parseExpression(QString);
		///parses and evaluates an expression string into a final value
		QVariant evaluate(QString);
		///evaluates a parsed expression into a final value
		QVariant evaluate(ELAMExpression);
		
		
	private:
		class Private;
		Private *d;
};

#endif
