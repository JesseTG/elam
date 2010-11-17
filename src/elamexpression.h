//ELAM main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_EXPRESSION_H
#define ELAM_EXPRESSION_H

#include "elamvalue.h"
#include "dptr.h"

namespace ELAM {

/**Represents a single token in a parsed expression.

Tokens are pretty stupid themselves - they just know their type, position, their original piece of text and an optional value (literals). They are used by the engine and expressions to transform text into executable expressions.*/
class Token
{
	DECLARE_DPTR(d)
	public:
		///The type of token
		enum Type {
			///invalid token
			Invalid,
			///a name: function, variable, or constant
			Name,
			///an operator (unary or binary)
			Operator,
			///opening parenthese
			ParOpen,
			///closing parenthese
			ParClose,
			///a comma - separating expressions in function calls
			Comma,
			///a literal value
			Literal,
			///white space chars, this is actually not used for tokens, but for parsing
			Whitespace,
		};
		///creates an empty/invalid token
		Token(Position pos=Position(-1,-1));
		///creates a token from a parsed piece of string
		Token(QString,Type,Position pos=Position(-1,-1));
		///creates a literal token
		Token(QString,QVariant,Position pos=Position(-1,-1));
		///returns the string content of the token
		QString content()const;
		///returns the type of token this is
		Type type()const;
		///for literals: returns the value
		QVariant literalValue()const;
		///returns the original position of the token
		Position position()const;
};

QDebug& operator<<(QDebug&,const Token&);

class Engine;
class Expression
{
	DECLARE_SHARED_DPTR(d)
	public:
		enum Type {
			Literal,
			Variable,
			Constant,
			Function,
			Parentheses,
			UnaryOp,
			BinaryOp,
			Exception,
		};
		Expression();
		Expression(Engine*parent,const QList<Token>&tokens);
		
		QVariant evaluate();
};

//end of namespace
};

#endif
