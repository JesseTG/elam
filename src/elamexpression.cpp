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
		DPTR_NAME(){type=Invalid;}
		QString cont;
		Type type;
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
	d->pos=pos;
	//genericise most types
	if(t&LiteralMask)d->type=Literal;else
	if(t&NameMask)d->type=Name;else
	if(t&OperatorMask)d->type=Operator;
	else d->type=t;
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
void Token::setSubType(Token::Type s)
{
	//only set if it becomes more specialized
	if(d->type&s)
		d->type=s;
}
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
		case Token::Function:dbg<<"Function";break;
		case Token::Constant:dbg<<"Constant";break;
		case Token::Variable:dbg<<"Variable";break;
		case Token::UnaryOp:dbg<<"Unary";break;
		case Token::BinaryOp:dbg<<"Binary";break;
		case Token::AssignmentOp:dbg<<"Assignment";break;
		default:dbg<<"Unknown:"<<(int)tok.type();break;
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
// Expression

class DPTR_CLASS_NAME(Expression):public SharedDPtr
{
	public:
		DPTR_NAME(){type=Exception;oppos=-1;}
		QPointer<Engine>parent;
		QList<Token>tokens;
		Type type;
		ELAM::Exception excep;
		QList<Expression>subexpr;
		int oppos;
};
DEFINE_SHARED_DPTR(Expression);

Expression::Expression()
{
	d->excep=ELAM::Exception(ELAM::Exception::ParserError,"Invalid Expression");
}

//reduce surrounding parentheses and whitespace
inline QList<Token>Expression::simplifyTokens(QList<Token> toks)
{
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

inline QList<Token>Expression::classifyTokens(QList<Token> toks)
{
	Engine*eng=d->parent;
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
			if(toks[i-1].type() & (Token::Parentheses | Token::ParOpen | Token::OperatorMask))
				t.setSubType(Token::UnaryOp);
			else{
				if(eng->isAssignment(toks[i].content()))
					t.setSubType(Token::AssignmentOp);
				else
					t.setSubType(Token::BinaryOp);
			}
		}
		//add
		ret<<t;
	}
	return ret;
}

inline QList<Token>Expression::reduceTokens(QList<Token> toks)
{
	toks=classifyTokens(simplifyTokens(toks));
	QList<Token>ret,sub;
	//copy and create hierarchy
	int pcnt=0;
	for(int i=0;i<toks.size();i++){
		//count parentheses
		if(toks[i].type()==Token::ParOpen){
			pcnt++;
			if(pcnt==1){
				if(i==0 || toks.value(i-1).type()!=Token::Function)
					ret<<Token("",Token::Parentheses,toks[i].position());
				sub.clear();
				continue;
			}
		}else
		if(toks[i].type()==Token::ParClose){
			pcnt--;
			if(pcnt==0){
				ret[ret.size()-1].setSubTokens(sub);
				sub.clear();
				continue;
			}
		}
		//collect tokens
		if(pcnt)
			sub<<toks[i];
		else
			ret<<toks[i];
	}
	return ret;
}

inline Exception Expression::scanForError(const QList< Token >& toks)
{
	//check for invalid tokens
	for(int i=0;i<toks.size();i++)
		if(toks[i].type()==Token::Invalid){
			return ELAM::Exception(Exception::ParserError, "invalid token", toks[i].position());
		}
	//scan for parentheses mismatch
	int pcnt=0;
	for(int i=0;i<toks.size();i++){
		//check parentheses
		if(toks[i].type()==Token::ParOpen)pcnt++;else
		if(toks[i].type()==Token::ParClose)pcnt--;
		if(pcnt<0){
			return ELAM::Exception(ELAM::Exception::ParserError, "parentheses mismatch", toks[i].position());
		}
	}
	if(pcnt!=0){
		return ELAM::Exception(ELAM::Exception::ParserError, "parentheses mismatch", toks[0].position());
	}
	//nothing found
	return ELAM::Exception(ELAM::Exception::NoError);
}

Expression::Expression(Engine* parent, const QList< Token >& toks)
{
	//check for simple errors
	ELAM::Exception ex=scanForError(toks);
	if(ex.errorType()!=ELAM::Exception::NoError){
		d->type=Exception;
		d->excep=ex;
		d->tokens=toks;
		return;
	}
	d->parent=parent;
	d->tokens=reduceTokens(toks);
	qDebug()<<"tokens:"<<d->tokens;
	//check for nothing and complain
	if(d->tokens.size()==0){
		d->type=Exception;
		d->excep=ELAM::Exception(ELAM::Exception::ParserError,"no tokens", (toks.size()>0?toks[0].position():Position()));
		return;
	}
	//order 1: check for simplicity (literals, vars, consts)
	if(d->tokens.size()==1){
		switch(d->tokens[0].type()){
			case Token::Function:
				functionInit();
				d->type=(Type)d->tokens[0].type();
				break;
			case Token::Parentheses:
				d->subexpr<<Expression(parent,d->tokens[0].subTokens());
				d->type=(Type)d->tokens[0].type();
				break;
			case Token::Constant:
			case Token::Variable:
			case Token::Literal:
				d->type=(Type)d->tokens[0].type();
				break;
			default:
				qDebug()<<"expression in single token mode: unexpected token" <<d->tokens[0];
				d->type=Exception;
				d->excep=ELAM::Exception(ELAM::Exception::ParserError, "unexpected token", d->tokens[0].position());
				break;
		}
		return;
	}
	//search for assignment
	for(int i=0;i<d->tokens.size();i++){
		if(d->tokens[i].type()==Token::AssignmentOp){
			//check 1: position is 1
			if(i!=1){
				d->type=Exception;
				d->excep=ELAM::Exception(ELAM::Exception::ParserError, "invalid assignment", d->tokens[i].position());
				return;
			}
			//check 2: pos 0 is variable
			if(d->tokens[0].type()!=Token::Variable){
				d->type=Exception;
				d->excep=ELAM::Exception(ELAM::Exception::OperationError, "left side of assignment must be a variable", d->tokens[i].position());
				return;
			}
			//check 3: there is a right
			if(d->tokens.size()<3){
				d->type=Exception;
				d->excep=ELAM::Exception(ELAM::Exception::OperationError, "assignment must have right side expression", d->tokens[i].position());
				return;
			}
			//ok
			d->subexpr<<Expression(parent,d->tokens.mid(2));
			return;
		}
	}
	//search for lowest operator on the right
	int cprio=1000,cpos=-1;
	for(int i=0;i<d->tokens.size();i++){
		if(d->tokens[i].type()==Token::BinaryOp){
			int oprio=d->parent->binaryOperatorPrio(d->tokens[i].content());
			if(oprio<=cprio){
				cprio=oprio;
				cpos=i;
			}
		}else if(d->tokens[i].type()==Token::UnaryOp){
			if(cprio>100){
				cprio=100;
				cpos=i;
			}
		}
	}
	//validity check
	if(cpos<0){
		d->type=Exception;
		d->excep=ELAM::Exception(ELAM::Exception::ParserError, "invalid expression", d->tokens[0].position());
		return;
	}
	//split operation
	d->oppos=cpos;
	if(d->tokens[cpos].type()==Token::BinaryOp){
		d->subexpr<<Expression(parent,d->tokens.mid(0,cpos));
		d->subexpr<<Expression(parent,d->tokens.mid(cpos+1));
	}else{
		d->subexpr<<Expression(parent,d->tokens.mid(cpos+1));
	}
}

void Expression::functionInit()
{
	QList<Token>sub=d->tokens[0].subTokens();
	QList<Token>par;
	for(int i=0;i<sub.size();i++){
		if(sub[i].type()==Token::Comma){
			d->subexpr<<Expression(d->parent,par);
			par.clear();
		}
	}
	if(par.size()>0){
		d->subexpr<<Expression(d->parent,par);
	}
}


QVariant Expression::evaluate()
{
	if(d->type==Exception)return d->excep;
	if(d->parent.isNull())return ELAM::Exception(ELAM::Exception::OperationError,"Lost engine context, cannot evaluate.");
	return QVariant();
}

void printExpression(QDebug&dbg,const Expression&ex,int level)
{
	printspaces(dbg,level);
	dbg<<"Expression(type=";
	switch(ex.d->type){
		case Expression::Literal:dbg<<"Literal";break;
		case Expression::Variable:dbg<<"Variable";break;
		case Expression::Constant:dbg<<"Constant";break;
		case Expression::Function:dbg<<"Function";break;
		case Expression::Parentheses:dbg<<"Parentheses";break;
		case Expression::UnaryOp:dbg<<"UnaryOperator";break;
		case Expression::BinaryOp:dbg<<"BinaryOperator";break;
		case Expression::AssignmentOp:dbg<<"Assignment";break;
		case Expression::Exception:dbg<<"Exception";break;
		default:dbg<<"Unknown:"<<(int)ex.d->type;break;
	}
	if(ex.d->excep.errorType()!=ELAM::Exception::NoError)
		dbg<<",exception="<<ex.d->excep;
	if(ex.d->subexpr.size()>0){
		dbg<<",subexpressions:";
		for(int i=0;i<ex.d->subexpr.size();i++){
			dbg<<"\n";
			printExpression(dbg,ex.d->subexpr[i],level+1);
		}
	}
	dbg<<")";
}

QDebug& operator<<(QDebug&dbg,const Expression&ex)
{
	dbg.nospace();
	printExpression(dbg,ex,0);
	return dbg.space();
}


};