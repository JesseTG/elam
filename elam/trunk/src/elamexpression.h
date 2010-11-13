//ELAM main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_EXPRESSION_H
#define ELAM_EXPRESSION_H

#include "elamvalue.h"
#include "dptr.h"

namespace ELAM {

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

class Expression
{
	public:
		enum Type {
			Literal,
			Variable,
			Constant,
			Function,
			Parentheses,
			UnaryOp,
			BinaryOp,
		};
		Expression(){}
		Expression(const QList<Token>&){}
};

//end of namespace
};

#endif
