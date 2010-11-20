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
			///the token represents a function call
			Function = 1,
			///the token represents a constant
			Constant = 2,
			///the token represents a variable
			Variable = 4,
			///a name: function, variable, or constant
			Name=0xff,
			///mask for name tokens
			NameMask=Name,
			
			///the token represents an unary operator
			UnaryOp = 0x100,
			///the token represents a binary operator
			BinaryOp = 0x200,
			///the token represents an assignment (with optional implicit binary op)
			AssignmentOp = 0x400,
			///an operator (unary or binary)
			Operator=0xff00,
			///mask for operator tokens
			OperatorMask=Operator,
			
			///mask for special class tokens that have a syntactic function (parentheses, comma, ...)
			SpecialCharMask=0xff0000,
			///mask for parentheses
			ParenthesesMask=0xf0000,
			///meta-type used for parsed sub-tokens
			Parentheses=0x10000,
			///opening parenthese
			ParOpen=0x20000,
			///closing parenthese
			ParClose=0x40000,
			///a comma - separating expressions in function calls
			Comma=0x100000,
			
			///a literal value
			Literal=0x1000000,
			///mask for literal values
			LiteralMask=Literal,
			
			///mask for tokens that are functional (non-ignored)
			FunctionalMask=NameMask|OperatorMask|SpecialCharMask,
			
			///white space chars, this is actually not used for tokens, but for parsing
			Whitespace=0x10000000,
			///mask for ignored tokens
			IgnoredTokenMask=0xf0000000,
		};
		///creates an empty/invalid token
		Token(Position pos=Position(-1,-1));
		///creates a token from a parsed piece of string, only generic types can be used
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
		///when the token is hierarchised (Parentheses, Function) this returns the subordinate tokens
		QList<Token>subTokens()const;
		
		///true if the token is functional (not ignored)
		inline bool isFunctional()const{return type()&FunctionalMask;}
		///true if the token represents an operator
		inline bool isOperator()const{return type()&OperatorMask;}
		///true if the token can be ignored
		inline bool isIgnored()const{return type()&IgnoredTokenMask;}
		///true if the token represents a name
		inline bool isName()const{return type()&NameMask;}
		///true if the token is a literal
		inline bool isLiteral()const{return type()&LiteralMask;}
	protected:
		friend class Expression;
		void setSubType(Type);
		void addSubToken(const Token&);
		void setSubTokens(const QList<Token>&);
};

QDebug& operator<<(QDebug&,const Token&);

class Engine;
/**Represents an expression in the context of its engine.

An expression always has an engine as context and stores a hierarchy of operations (also expressions) to perform when called to evaluate. Expressions may become invalid if constants, functions, or operators change between the time they are created and the time they are evaluated.
*/
class Expression
{
	DECLARE_SHARED_DPTR(d)
	public:
		enum Type {
			Literal=Token::Literal,
			Variable=Token::Variable,
			Constant=Token::Constant,
			Function=Token::Function,
			Parentheses=Token::Parentheses,
			UnaryOp=Token::UnaryOp,
			BinaryOp=Token::BinaryOp,
			AssignmentOp=Token::AssignmentOp,
			Exception=Token::IgnoredTokenMask,
		};
		///creates and invalid expression, that always evaluates to an exception
		Expression();
		///creates an expression by its context engine and its tokens
		Expression(Engine*parent,const QList<Token>&tokens);
		
		///evaluates the expression and returns the result of the evaluation
		QVariant evaluate();
	private:
		///scan tokens and decide what specific sub-type they are
		QList<Token>classifyTokens(QList<Token> toks);
		/**pushes parentheses and function arguments into the sub-tokens of their parents;
		the result is a list of tokens that are on the same level*/
		QList<Token>reduceTokens(QList<Token> toks);
		///scan tokens for errors, returns Exception::NoError if none are found
		ELAM::Exception scanForError(const QList< Token >& toks);
		///reduce surrounding parentheses and whitespace
		QList<Token>simplifyTokens(QList<Token> toks);
		///parses tokens and splits them by comma
		void functionInit();
};

//end of namespace
};

#endif
