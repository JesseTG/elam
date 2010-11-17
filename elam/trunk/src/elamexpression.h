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
	DECLARE_SHARED_DPTR(d)
	public:
		///The type of token
		enum Type {
			///invalid token
			Invalid=0,
			///a name: function, variable, or constant
			Name=7,
			///an operator (unary or binary)
			Operator=24,
			///meta-type used for parsed sub-tokens
			Parentheses=96,
			///opening parenthese
			ParOpen=32,
			///closing parenthese
			ParClose=64,
			///a comma - separating expressions in function calls
			Comma=128,
			///a literal value
			Literal=256,
			///white space chars, this is actually not used for tokens, but for parsing
			Whitespace=512,
		};
		enum SubType{
			None = 0,
			Function = 1,
			Constant = 2,
			Variable = 4,
			UnaryOp = 8,
			BinaryOp = 16,
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
		SubType subType()const;
		void setSubType(SubType);
		QList<Token>subTokens()const;
		void addSubToken(const Token&);
		void setSubTokens(const QList<Token>&);
};

QDebug& operator<<(QDebug&,const Token&);

class Engine;
class Expression
{
	DECLARE_SHARED_DPTR(d)
	public:
		enum Type {
			Literal=Token::Literal,
			Variable=Token::Variable,
			Constant=Token::Constant,
			Function=Token::Function,
			Parentheses=96,
			UnaryOp=Token::UnaryOp,
			BinaryOp=Token::BinaryOp,
			Exception=0x8000,
		};
		Expression();
		Expression(Engine*parent,const QList<Token>&tokens);
		
		QVariant evaluate();
};

//end of namespace
};

#endif
