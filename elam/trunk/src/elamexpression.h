//ELAM main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_EXPRESSION_H
#define ELAM_EXPRESSION_H

#include "elamvalue.h"

class ELAMToken
{
	public:
		enum Type {
			Name,
			Operator,
			ParOpen,
			ParClose,
			Comma,
			Literal
		};
		QString content()const{return mcont;}
		Type type()const{return mtype;}
	private:
		QString mcont;
		Type mtype;
};

class ELAMExpression
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
};

#endif
