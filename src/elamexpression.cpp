#include "elamexpression.h"

#include <QDebug>

namespace ELAM {

///////////////////////////////////////////////////////////////////////////////
// Token

class DPTR_CLASS_NAME(Token):public DPtr
{
	public:
		QString cont;
		Type type;
		QVariant val;
		Position pos;
};
DEFINE_DPTR(Token)


Token::Token(Position pos)
{
	d->pos=pos;
	d->type=Invalid;
}

Token::Token(QString c,Token::Type t,Position pos)
{
	d->cont=c;
	d->type=t;
	d->pos=pos;
}

Token::Token(QString c,QVariant v,Position pos)
{
	d->cont=c;
	d->val=v;
	d->pos=pos;
	d->type=Literal;
}

QString Token::content()const{return d->cont;}
Token::Type Token::type()const{return d->type;}
QVariant Token::literalValue()const{return d->val;}
Position Token::position()const{return d->pos;}

QDebug&operator<<(QDebug&dbg,const Token&tok)
{
	dbg.nospace()<<"Token(str="<<tok.content()<<",type=";
	switch(tok.type()){
		case Token::Invalid:dbg<<"Invalid";break;
		case Token::Name:dbg<<"Name";break;
		case Token::Operator:dbg<<"Operator";break;
		case Token::ParClose:dbg<<"ClosingParenthesis";break;
		case Token::ParOpen:dbg<<"OpeningParenthesis";break;
		case Token::Comma:dbg<<"Comma";break;
		case Token::Literal:dbg<<"LiteralValue"<<",value="<<tok.literalValue();break;
		case Token::Whitespace:dbg<<"WhiteSpace";break;
	}
	dbg<<",pos="<<tok.position()<<")";
	return dbg.space();
}

///////////////////////////////////////////////////////////////////////////////
// Expression

class DPTR_CLASS_NAME(Expression):public SharedDPtr
{
};
DEFINE_SHARED_DPTR(Expression);

Expression::Expression()
{

}
Expression::Expression(Engine* parent, const QList< Token >& tokens)
{

}

QVariant Expression::evaluate()
{
	return QVariant();
}



};