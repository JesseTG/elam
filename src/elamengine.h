// main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef _ENGINE_H
#define _ENGINE_H

#include <QStringList>
#include <QObject>
#include <QVariant>

#include "elamexpression.h"

#include "dptr.h"

namespace ELAM {

/**pointer to a function wrapping an unary operator
\param op the operand to be worked on
\returns the result of the operation*/
typedef QVariant (*UnaryOperatorCall)(const QVariant&op);

class UnaryOperator
{
	DECLARE_SHARED_DPTR(d);
	public:
		UnaryOperator(const UnaryOperator&);
		UnaryOperator();
		~UnaryOperator();
		
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
};

/**pointer to a function wrapping a binary operator
\param op1 the left operand
\param op2 the right operand
\returns the result of the operation*/
typedef QVariant (*BinaryOperatorCall)(const QVariant&op1,const QVariant&op2);

class BinaryOperator
{
	DECLARE_SHARED_DPTR(d);
	public:
		BinaryOperator(const BinaryOperator&);
		BinaryOperator();
		~BinaryOperator();
};

/**pointer to a function wrapping a mathematical function
\param args the list of arguments
\returns the result of the function*/
typedef QVariant (*Function)(const QList<QVariant>&args);

class CharacterClassSettings
{
	DECLARE_SHARED_DPTR(d)
	public:
		QString operatorClass()const;
		void setOperatorClass(QString);
		
		QPair<QString,QString> nameClass()const;
		void setNameClass(QString startchars,QString allchars);
		
		QString whitespaceClass()const;
		void setWhitespaceClass(QString);
		
		QString literalStartClass()const;
		void setLiteralStartClass(QString);
		
		QPair<QChar,QChar> parenthesesChars()const;
		void setParentheses(QChar,QChar);
		
		QPair<QChar,QChar> assignmentChars()const;
		void setAssignmentChars(QChar,QChar);
		
		QChar commaChar()const;
		void setCommaChar(QChar);
		
		bool isConsistent()const;
		
		Token::Type charType(QChar,Token::Type)const;
};

/**The calculation engine of .

Instances of this class can be configured to represent a specific system of functions and operators.

Methods of this class are used to evaluate expressions into final values.
*/
class Engine:public QObject
{
	Q_OBJECT
	DECLARE_DPTR(d);
	public:
		///instantiates an engine object
		Engine(QObject*parent=0);
		
	public slots:
		///simply parses an expression string into an  object
		Expression expression(QString);
		///simply parses an expression string into an  object
		Expression expression(QList<Token>);
		///simply parses an expression string into a list of tokens
		QList<Token> tokenize(QString);
		///parses and evaluates an expression string into a final value
		QVariant evaluate(QString);
		///evaluates a parsed expression into a final value
		QVariant evaluate(Expression);
		///gives access to the character classes settings
		CharacterClassSettings characterClasses();
};

//end of namespace
};

#endif
