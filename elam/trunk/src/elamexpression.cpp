#include "elamexpression.h"

#include <QDebug>
#include <QPointer>
#include "elamengine.h"

namespace ELAM {

///////////////////////////////////////////////////////////////////////////////
// Token

class DPTR_CLASS_NAME(Token):public SharedDPtr
{
	public:
		DPTR_NAME(){type=Invalid;subtype=None;}
		QString cont;
		Type type;
		SubType subtype;
		QVariant val;
		Position pos;
		QList<Token>subtok;
};
DEFINE_SHARED_DPTR(Token)


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

QList< Token > Token::subTokens() const{return d->subtok;}
Token::SubType Token::subType() const{return d->subtype;}
void Token::setSubType(Token::SubType s){d->subtype=s;}
void Token::addSubToken(const ELAM::Token& t)
{
	d->subtok<<t;
}
void Token::setSubTokens(const QList< Token >& t)
{
	d->subtok=t;
}

static void printspaces(QDebug&dbg,int level)
{
	for(int i=0;i<level;i++)dbg<<"   ";
}

static void printtoken(QDebug&dbg,const Token&tok,int level);
static void printtokenlist(QDebug&dbg,const QList<Token>&tok,int llevel,int tlevel)
{
	printspaces(dbg,llevel);
	dbg<<"TokenList(";
	for(int i=0;i<tok.size();i++){
		dbg<<"\n";
		printtoken(dbg,tok[i],tlevel+1);
	}
	dbg<<")";
}
static void printtoken(QDebug&dbg,const Token&tok,int level)
{
	printspaces(dbg,level);
	dbg<<"Token(str="<<tok.content()<<",type=";
	switch(tok.type()){
		case Token::Invalid:dbg<<"Invalid";break;
		case Token::Name:dbg<<"Name";break;
		case Token::Operator:dbg<<"Operator";break;
		case Token::ParClose:dbg<<"ClosingParenthesis";break;
		case Token::ParOpen:dbg<<"OpeningParenthesis";break;
		case Token::Comma:dbg<<"Comma";break;
		case Token::Literal:dbg<<"LiteralValue,value="<<tok.literalValue();break;
		case Token::Whitespace:dbg<<"WhiteSpace";break;
		case Token::Parentheses:dbg<<"Parentheses";break;
	}
	if(tok.subType()!=Token::None){
		dbg<<",subtype=";
		switch(tok.subType()){
			case Token::Function:dbg<<"Function";break;
			case Token::Constant:dbg<<"Constant";break;
			case Token::Variable:dbg<<"Variable";break;
			case Token::UnaryOp:dbg<<"Unary";break;
			case Token::BinaryOp:dbg<<"Binary";break;
			default:break;
		}
	}
	dbg<<",pos="<<tok.position();
	QList<Token> sub=tok.subTokens();
	if(sub.size()>0){
		dbg<<",sub-tokens:";
		printtokenlist(dbg,sub,0,level);
	}
	dbg<<")";
}
QDebug&operator<<(QDebug&dbg,const Token&tok)
{
	dbg.nospace();
	printtoken(dbg,tok,0);
	return dbg.space();
}

QDebug&operator<<(QDebug&dbg,const QList<Token>&tok)
{
	dbg.nospace();
	printtokenlist(dbg,tok,0,0);
	return dbg.space();
}
///////////////////////////////////////////////////////////////////////////////
// TokenBundle

class TokenBundle
{
	public:
		TokenBundle(const Token&);
		TokenBundle(const TokenBundle&);
		TokenBundle(const QList<Token>&);
	private:
		QList<TokenBundle>&mtoks;
};

///////////////////////////////////////////////////////////////////////////////
// Expression

class DPTR_CLASS_NAME(Expression):public SharedDPtr
{
	public:
		DPTR_NAME(){type=Exception;}
		QPointer<Engine>parent;
		QList<Token>tokens;
		Type type;
		QVariant value;
};
DEFINE_SHARED_DPTR(Expression);

Expression::Expression()
{

}

//reduce surrounding parentheses and whitespace
static inline QList<Token>simplifyTokens(Engine*eng,QList<Token> toks)
{
	Q_UNUSED(eng);
	QList<Token>ret;
	int min=0,max=toks.size()-1;
	//eliminate redundant parentheses
	while(toks[min].type()==Token::ParOpen && toks[max].type()==Token::ParClose){
		min++;max--;
	}
	//reduce whitespace
	for(int i=min;i<=max;i++){
		//eliminate whitespace
		if(toks[i].type()==Token::Whitespace)
			continue;
		ret<<toks[i];
	}
	return ret;
}

//reduce surrounding parentheses and whitespace
static inline QList<Token>classifyTokens(Engine*eng,QList<Token> toks)
{
	QList<Token>ret;
	if(toks.size()<1)return toks;
	//check token 0
	Token t=toks[0];
	if(t.type()==Token::Name){
		if(eng->hasFunction(t.content()))t.setSubType(Token::Function);else
		if(eng->hasConstant(t.content()))t.setSubType(Token::Constant);
		else t.setSubType(Token::Variable);
	}else if(t.type()==Token::Operator)t.setSubType(Token::UnaryOp);
	ret<<t;
	//check remaining tokens
	for(int i=1;i<toks.size();i++){
		t=toks[i];
		//define names
		if(t.type()==Token::Name){
			if(eng->hasFunction(t.content()))t.setSubType(Token::Function);else
			if(eng->hasConstant(t.content()))t.setSubType(Token::Constant);
			else t.setSubType(Token::Variable);
		}else
		//define operators
		if(t.type()==Token::Operator){
			switch(toks[i-1].type()){
				case Token::ParOpen:
				case Token::Operator:
					t.setSubType(Token::UnaryOp);
					break;
				default:
					t.setSubType(Token::BinaryOp);
					break;
			}
		}
		//add
		ret<<t;
	}
	return ret;
}
//reduce surrounding parentheses and whitespace
static inline QList<Token>reduceTokens(Engine*eng,QList<Token> toks)
{
	toks=classifyTokens(eng,simplifyTokens(eng,toks));
	QList<Token>ret,sub;
	//copy and create hierarchy
	int pcnt=0;
	for(int i=0;i<toks.size();i++){
		//count parentheses
		if(toks[i].type()==Token::ParOpen){
			if(!pcnt){
				if(i==0 || toks.value(i-1).subType()!=Token::Function)
					ret<<Token("",Token::Parentheses,toks[i].position());
				sub.clear();
			}
			pcnt++;
			continue;
		}else
		if(toks[i].type()==Token::ParClose){
			pcnt--;
			if(!pcnt){
				ret[ret.size()-1].setSubTokens(sub);
				sub.clear();
			}
			continue;
		}
		//collect tokens
		if(pcnt)
			sub<<toks[i];
		else
			ret<<toks[i];
	}
	return ret;
}
//scan for simple errors, return exception (NoError if no error found)
static inline Exception scanForError(const QList< Token >& toks)
{
	//check for invalid tokens
	for(int i=0;i<toks.size();i++)
		if(toks[i].type()==Token::Invalid){
			return Exception(Exception::ParserError, "invalid token", toks[i].position());
		}
	//scan for parentheses mismatch
	int pcnt=0;
	for(int i=0;i<toks.size();i++){
		//check parentheses
		if(toks[i].type()==Token::ParOpen)pcnt++;else
		if(toks[i].type()==Token::ParClose)pcnt--;
		if(pcnt<0){
			return Exception(Exception::ParserError, "parentheses mismatch", toks[i].position());
		}
	}
	if(pcnt!=0){
		return Exception(Exception::ParserError, "parentheses mismatch", toks[0].position());
	}
	//nothing found
	return Exception(Exception::NoError);
}

Expression::Expression(Engine* parent, const QList< Token >& toks)
{
	//check for simple errors
	ELAM::Exception ex=scanForError(toks);
	if(ex.errorType()!=ELAM::Exception::NoError){
		d->type=Exception;
		d->value=ex;
		d->tokens=toks;
		return;
	}
	d->parent=parent;
	d->tokens=reduceTokens(parent,toks);
	qDebug()<<"expression:"<<d->tokens;
	//check for nothing and complain
	if(d->tokens.size()==0){
		d->type=Exception;
		d->value=ELAM::Exception(ELAM::Exception::ParserError,"no tokens", (toks.size()>0?toks[0].position():Position()));
		return;
	}
	//check for simplicity (literals, vars, consts)
	if(d->tokens.size()==1){
		switch(d->tokens[0].type()){
			case Token::Name:
				if(parent->hasFunction(d->tokens[0].content())){
					d->type=Exception;
					d->value=ELAM::Exception(ELAM::Exception::ParserError, "function call incomplete", d->tokens[0].position());
				}else if(parent->hasConstant(d->tokens[0].content())){
					d->type=Constant;
					d->value=parent->getConstant(d->tokens[0].content());
				}else{
					d->type=Variable;
				}
				break;
			case Token::Literal:
				d->type=Literal;
				d->value=d->tokens[0].literalValue();
				break;
			default:
				d->type=Exception;
				d->value=ELAM::Exception(ELAM::Exception::ParserError, "unexpected token", d->tokens[0].position());
				break;
		}
		return;
	}
}

QVariant Expression::evaluate()
{
	return QVariant();
}



};