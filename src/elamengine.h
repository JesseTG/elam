// main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_ENGINE_H
#define ELAM_ENGINE_H

#include <QStringList>
#include <QObject>
#include <QVariant>

#include "elamexpression.h"
#include "elamunary.h"
#include "elambinary.h"
#include "elamcharclass.h"

#include "dptr.h"

namespace ELAM {


/**pointer to a function wrapping a mathematical function
\param args the list of arguments
\returns the result of the function or ELAM::Exception in case or errors

Functions must check their arguments for validity before they start calculating. On error a function should returns an ELAM::Exception.
*/
typedef QVariant (*Function)(const QList<QVariant>&args);


class Engine;

/**wraps the parser routine for a literal
\param expr the original expression string
\param engine the engine that is calling this parser
\param start the character (index of expr) at which the literal starts
\returns the parsed literal (first=string representation, second=value represented by it

The string representation of the literal must be verbatim from expr, so that the calling engine can determine where to continue parsing in expr.

If the parser does not find a valid literal according to its rules it must return an empty string.
*/
typedef QPair<QString,QVariant> (*LiteralParser)(const QString&expr,Engine&engine,int start);

/**Wraps a cast function to convert various types into another type.
\param orig the original value of any type
\returns the converted value that must conform to the expected type*/
typedef QVariant (*TypeCast)(const QVariant&orig);

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
		
		///true if the named variable exists in this engine
		Q_INVOKABLE bool hasVariable(QString)const;
		///true if the named constant exists in this engine
		Q_INVOKABLE bool hasConstant(QString)const;
		///true if a variable or constant of that name exists in this engine
		Q_INVOKABLE bool hasValue(QString)const;
		
		///returns true if the named function exists
		Q_INVOKABLE bool hasFunction(QString)const;
		///returns the pointer to the function
		Q_INVOKABLE Function getFunction(QString)const;
		
		///returns true if there is an unaryOperator with this name
		Q_INVOKABLE bool hasUnaryOperator(QString name)const;
		///returns true if there is an binaryOperator with this name
		Q_INVOKABLE bool hasBinaryOperator(QString name)const;

		/** \brief returns an existing or new unary operator object
		\param name the name token of the operator, if the name is not a valid operator it cannot be called from this engine until character classes change*/
		Q_INVOKABLE UnaryOperator unaryOperator(QString name);
		
		/**Matching mode for priority when overwriting an operator, see binaryOperator
		
		The matching mode has no effect if the operator is new or the priorities match. In those cases binaryOperator always returns the operator object corresponding to the given name for this engine.
		
		The matching mode changes behavior if the operator exists and its priority is different from the one given in the call.
		*/
		enum PriorityMatch{
			///if the operator is new: use the given priority, otherwise leave priority as is ignoring the one given in the call, this is the default
			IgnoreMismatch=0,
			///override the priority of the existing operator, using the priority given in the call
			OverridePrio=1,
			///returns a dummy operator making it impossible to overwrite the existing operator
			FailIfMismatch=2,
		};
		
		/** \brief returns an existing or new binary operator object
		\param name the name token of the operator, if the name is not a valid operator it cannot be called from this engine until character classes change
		\param prio the priority that should be set for the operator
		\param match how to behave if the operator already exists and priorities do not match, see PriorityMatch
		\returns a reference to the operator, or to a dummy operator if priority matching failed
		*/
		Q_INVOKABLE BinaryOperator binaryOperator(QString name,int prio=1,PriorityMatch match=IgnoreMismatch);
		
		///returns the priority of the operator, or -1 if the operator does not exist
		Q_INVOKABLE int binaryOperatorPrio(QString name);
		
		///returns true if the name represents an assignment operator
		Q_INVOKABLE bool isAssignment(QString name)const;

		/**performs automatic casting
		\returns the casted value or the original value if it is of a primary type or if there is no known cast for it*/
		Q_INVOKABLE QVariant autoCast(const QVariant&)const;

	public slots:
		///returns the value of the named variable or constant
		QVariant getValue(QString)const;
		///returns the value of the named variable (does not return constants)
		QVariant getVariable(QString)const;
		///returns the value of the named constant (does not return variables)
		QVariant getConstant(QString)const;
		
		/**sets a variable
		
		\returns true on success or false if:
		 - the name is not valid
		 - a function or constant of that name already exists
		*/
		bool setVariable(QString,QVariant);
		/**sets a constant
		
		\returns true on success or false if:
		 - the name is not valid
		 - a function of that name exists
		
		Constants overwrite variables - if a variable of the same name exists, it is transparently deleted before the constant is created.*/
		bool setConstant(QString,QVariant);

		///deletes a variable (does not affect constants)
		void removeVariable(QString);
		///deletes a constant (does not affect variables)
		void removeConstant(QString);
		///deletes a variable or constant
		void removeValue(QString);
		
		/**sets the function
		\returns true on success or false if:
		 - the name is not valid
		 - the function is null
		 - a constant or variable of the same name exists already*/
		bool setFunction(QString,Function);
		///removes a function
		void removeFunction(QString);

		/**sets the parser routine for a literal value
		\param parser pointer to the parser routine
		\param startchars characters that the literal can start with, at least some of those characters must be part of the literalStart class, the ones which are not part of it will be ignored when recognizing a literal - if none are part of the class the literal cannot be used until the class changes
		\param prio a value between 0 and 100, parsers with higher values are preferred over those with lower values if they share a start character
		\returns true if the parser is (re-)registered successfully, or false if:
		 - the parser is null
		 - the start characters are empty
		 - the priority is outside the allowed range (0<=prio<=100)
		
		If a parser function is registered a second time the new registration overwrites the old registration.
		*/
		bool setLiteralParser(LiteralParser parser,QString startchars,int prio=50);
		///removes a parser function
		void removeLiteralParser(LiteralParser parser);
		
		///sets/overrides the priority of an operator, creating the operator if it does not exist yet
		void setBinaryOperatorPrio(QString name,int prio);
		
		/**registers a type as primary, this means it is not cast into another type when encountered
		\param typeId the QVariant ID of the type to register*/
		void registerType(int typeId);
		/**Registers an automatic cast function.
		
		Automatic cast functions must succeed in converting, if the result is uncertain (e.g. as in converting a string to a number) use an explicit function.
		
		\param target the target of the cast function, the target is automatically registered as primary type (see registerType)
		\param origin type IDs that are automatically converted using this function, origins that are also primary types are ignored
		\param castfunc the function that converts these types
		\param prio the priority of the cast, if a cast function for the same origin, but a higher priority exists the one with the higher priority is used; the priority must be a positive value*/
		void setAutoCast(int target,QList<int>origin,TypeCast castfunc,int prio=50);
		
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
	private:
		///parse a literal
		QPair<QString,QVariant>parseLiteral(QString,int);
};

//end of namespace
};

#endif
