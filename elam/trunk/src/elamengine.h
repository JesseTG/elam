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
};

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
};

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

/** \brief This class holds the character classes used by an Engine.

\see Engine::characterClasses()

There are three major classes of chracters:
 - names
  - names can be variables, constants, functions
  - the names class has two sub-classes
   - start-of-names is characters that may start a name, usually letters and underscores
   - all-of-names is characters that may be contained anywhere in names, this class must include the complete start of names class
 - operators
  - any sequence of these characters is interpreted as unary or binary operator
 - whitespace
  - whitespace can separate other tokens, but is ignored itself

None of these three classes may overlap. There are several more minor classes:
 - literals
  - the character class actually only contains characters that can start a literal, the continuation and end of literals is determines by a callback routine
  - characters in this class may overlap with names, but must not overlap with operators, whitespace and start-of-names
 - parentheses
  - are two distinct characters that group expressions, none of them must be contained in any other class
 - comma
  - a single character separating arguments in functions, the comma character must not be contained in any other class
 - assignment
  - one or two characters that denote an assignment
  - both are optional, both must be contained in operators
  - one of the characters, if it exists, is the first character of an assignment
  - the other of the characters, if it exists, is the last character of an assignment
  - just those two characters are the simple assignment operator
  - both surrounding another operator combine the operation with assignment (e.g. a+=b is equivalent to a=a+b)

*/
class CharacterClassSettings
{
	DECLARE_SHARED_DPTR(d)
	public:
		///class of operator characters
		QString operatorClass()const;
		///sets the class of operator characters
		void setOperatorClass(QString);
		
		/**returns the two name sub-classes
		nameClass().first refers to start characters, 
		nameClass().second refers to all characters that can turn up anywhere in names*/
		QPair<QString,QString> nameClass()const;
		/**sets the two sub-classes of names
		\param startchars are characters that can start a name
		\param allchars are characters that can turn up anywhere in names, allchars must include all characters from startchars*/
		void setNameClass(QString startchars,QString allchars);
		
		///returns all characters that are regarded as whitespace
		QString whitespaceClass()const;
		///sets all characters that are regarded as whitespace
		void setWhitespaceClass(QString);
		
		///returns all characters that start a literal, normally digits ' and "
		QString literalStartClass()const;
		///sets characters that can start a literal
		void setLiteralStartClass(QString);
		
		///returns the opening (first) and closing (second) character of parentheses
		QPair<QChar,QChar> parenthesesChars()const;
		/**sets the characters used for parentheses
		\param open the character that opens/begins a parentheses structure, normally "("
		\param close the character that closes/ends a parentheses structure, normally ")"
		
		The parentheses characters must not be included in any other class.*/
		void setParentheses(QChar open,QChar close);
		
		/** \brief returns the characters that designate an assignment
		
		- assignmentChars().first is the character that starts an assignment operator,
		- assignmentChars().second is the character that ends it.*/
		QPair<QChar,QChar> assignmentChars()const;
		
		/** sets the characters used for assignment operators
		\param start if not '\0' the character that marks the start of an assignment
		\param end if not '\0' the character that marks the end of an assignment
		
		The combination of both characters along (without whitespace) is the direct assignment operator. In automatic assignment operator mode any operator that starts with the start character and ends with end character is regarded as an implicit assignment.
		
		In the default configuration the start character is not set ('\0') and the end character is '=', so "a=1" will assign the value "1" to the variable "a" and "a += 1" is equivalent to "a = a + 1".
		
		You can turn this automatism around by defining a start character only (e.g. start='=' and end='\0'). Then the assignment would still be "a = 1", but the combination of assignment and "+" would become "a =+ 1".
		
		If you define both characters then both must be present in assignments. For example with start=':' and end='=' then assignment becomes "a:=1" and assignment with "+" becomes "a :+= 1".
		
		If set, both characters must be part of the operator class.
		
		If both start and end are '\0' it will be impossible to make assignments*/
		void setAssignmentChars(QChar start,QChar end);
		
		///returns the character used as a comma (separator of function arguments)
		QChar commaChar()const;
		/**sets the character used as comma
		
		The character must not be part of any other class.*/
		void setCommaChar(QChar);
		
		/**true if the settings are internally consistent
		
		The character class settings are consistent if all constraints of class inclusion and exclusion are fullfilled.*/
		bool isConsistent()const;
		
		/**returns the type of token the character belongs to
		\param ch the character to be checked
		\param oldtype the type the previous character belongs to (set to Invalid if this is the first character)
		
		Compares the character with the known settings for character classes and returns its probable token type. The old type of the previous character is necessary to check for context sensitive rules.
		
		\returns the type of this character:
		 - Invalid is returned if the character does not match any class, parsing should stop here
		 - If Literal is returned the calling engine must use specialized literal checking functions to find the end of the literal, the next character checke with this routine should be the first one behind the end of the literal
		 - Assignment characters are returned as Operator class
		 - Whitespace class characters should be ignored
		 - Special character types (Par*, Comma) must be tokenized separately
		 - Any other type (Name, Operator) must be concatenated until the return type changes
		*/
		Token::Type charType(QChar ch,Token::Type oldtype)const;
		
		///returns true if the string contains any assignment operator
		bool isAssignment(QString)const;
		///returns true if the string contains exactly the simple assignment operator
		bool isSimpleAssignment(QString)const;
		
		///returns true if the string can be interpreted as name
		bool isName(QString)const;
		
		///returns true if the string can be interpreted as operator
		bool isOperator(QString)const;
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
		
		///true if the named variable exists in this engine
		bool hasVariable(QString)const;
		///true if the named constant exists in this engine
		bool hasConstant(QString)const;
		///true if a variable or constant of that name exists in this engine
		bool hasValue(QString)const;
		
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
		
		///returns true if the named function exists
		bool hasFunction(QString)const;
		///returns the pointer to the function
		Function getFunction(QString)const;
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
		\param startchars characters that the literal can start with, all of those characters must be part of the literalStart class
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
	private:
		///parse a literal
		QPair<QString,QVariant>parseLiteral(QString,int);
};

//end of namespace
};

#endif
