//  engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamengine.h"
#include "elamvalue.h"

#include <QDebug>

namespace ELAM {
///////////////////////////////////////////////////////////////////////////////
// Engine

class DPTR_CLASS_NAME(Engine):public DPtr
{
	public:
		CharacterClassSettings cclass;
		QMap<QString,QVariant> vars,consts;
		QMap<QString,Function> funcs;
		struct LiteralParser_s
		{
			LiteralParser parser;
			QString start;
			int prio;
			//this operator is reversed, so that highest prio is in element 0 after qSort
			bool operator<(const LiteralParser_s&l2)const
			{return prio>l2.prio;}
		};
		QList<LiteralParser_s>parsers;
		QMap<QString,UnaryOperator>unary;
		struct BinaryOperator_s {
			BinaryOperator_s(int p=1){prio=p;}
			BinaryOperator_s(BinaryOperator o,int p){op=o;prio=p;}
			BinaryOperator op;
			int prio;
		};
		QMap<QString,BinaryOperator_s>binary;
		struct CastFunc_s {
			CastFunc_s(int p=50){prio=p;target=-1;cfunc=0;}
			int target,prio;
			QList<int>origin;
			TypeCast cfunc;
		};
		QList<CastFunc_s>casts;
		QList<int>primtypes;
};
DEFINE_DPTR(Engine);

Engine::Engine(QObject* parent): QObject(parent)
{
	registerType(Exception::metaTypeId());
}

QVariant Engine::evaluate(QString ex)
{
	return evaluate(expression(ex));
}

QVariant Engine::evaluate(Expression ex)
{
	return ex.evaluate();
}

Expression Engine::expression(QString s)
{
	return Expression(this,tokenize(s));
}

Expression Engine::expression(QList< Token > tok)
{
	return Expression(this,tok);
}

QList< Token > Engine::tokenize(QString ex)
{
	//presets
	Token::Type ctype=Token::Invalid;
	QString ctok;
	QList<Token> ret;
	int cline=1,ccol=0;
	int sline=cline,scol=ccol;
	int litend=-1;
	//go through expression
	for(int i=0;i<ex.size();i++){
		if(ex[i]=='\n'){cline++;ccol=0;}
		ccol++;
		//skip everything till end of a literal
		if(litend>i)continue;
		//get type of current character
		Token::Type ntype=d->cclass.charType(ex[i],ctype);
		//check for invalid stuff
		if(ntype==Token::Invalid)
			return QList<Token>()<<Token(Position(cline,ccol));
		//check whether we are still inside the same token
		if(ntype==ctype)
			ctok+=ex[i];
		else{
			//store old token
			if(ctype!=Token::Invalid && ctype!=Token::Whitespace && ctok!="")
				ret<<Token(ctok,ctype,Position(sline,scol));
			//reset state
			ctok.clear();
			sline=cline;
			scol=ccol;
			//handle current char
			switch(ntype){
				case Token::ParOpen:
				case Token::ParClose:
				case Token::Comma:
					//special chars
					ret<<Token(QString(ex[i]),ntype,Position(cline,ccol));
					ctype=Token::Invalid;
					break;
				case Token::Whitespace:
				case Token::Operator:
				case Token::Name:
					//start new token
					ctype=ntype;
					ctok+=ex[i];
					break;
				case Token::Literal:{
					//parse it
					QPair<QString,QVariant>lt=parseLiteral(ex,i);
					//check for failure
					if(lt.first=="")
						return QList<Token>()<<Token(Position(cline,ccol));
					//store token
					ret<<Token(lt.first,lt.second,Position(cline,ccol));
					//make the loop skip the rest
					litend=i+lt.first.size();
					ctype=Token::Invalid;
					break;}
				default://nothing
					qDebug()<<"oops. unexpected token type at line"<<cline<<"col"<<ccol;
					ctype=Token::Invalid;
					break;
			}
		}
	}
	//add remaining stuff
	if(ctype!=Token::Invalid && ctype!=Token::Whitespace && ctok!="")
		ret<<Token(ctok,ctype,Position(sline,scol));
	return ret;
}

CharacterClassSettings Engine::characterClasses()
{
	return d->cclass;
}

QVariant Engine::getConstant(QString n) const
{
	if(d->consts.contains(n))return d->consts[n];
	return QVariant();
}

QVariant Engine::getVariable(QString n) const
{
	if(d->vars.contains(n))return d->vars[n];
	return QVariant();
}

QVariant Engine::getValue(QString n) const
{
	if(d->consts.contains(n))return d->consts[n];
	if(d->vars.contains(n))return d->vars[n];
	return QVariant();
}

bool Engine::hasConstant(QString n) const
{
	return d->consts.contains(n);
}

bool Engine::hasVariable(QString n) const
{
	return d->vars.contains(n);
}

bool Engine::hasValue(QString n) const
{
	return d->vars.contains(n) || d->consts.contains(n);
}

void Engine::removeConstant(QString n)
{
	d->consts.remove(n);
}

void Engine::removeVariable(QString n)
{
	d->vars.remove(n);
}

void Engine::removeValue(QString n)
{
	d->consts.remove(n);
	d->vars.remove(n);
}

bool Engine::setConstant(QString n, QVariant v)
{
	if(!d->cclass.isName(n))return false;
	if(d->funcs.contains(n))return false;
	d->vars.remove(n);
	d->consts.insert(n,autoCast(v));
	return true;
}

bool Engine::setVariable(QString n, QVariant v)
{
	if(!d->cclass.isName(n))return false;
	if(d->consts.contains(n))return false;
	if(d->funcs.contains(n))return false;
	d->vars.insert(n,autoCast(v));
	return true;
}

bool Engine::hasFunction(QString n) const
{
	return d->funcs.contains(n);
}

Function Engine::getFunction(QString n) const
{
	if(d->funcs.contains(n))return d->funcs[n];
	return 0;
}

bool Engine::setFunction(QString n, ELAM::Function p)
{
	if(p==0)return false;
	if(!d->cclass.isName(n))return false;
	if(d->consts.contains(n))return false;
	if(d->vars.contains(n))return false;
	d->funcs.insert(n,p);
	return true;
}

void Engine::removeFunction(QString n)
{
	d->funcs.remove(n);
}

bool Engine::setLiteralParser ( ELAM::LiteralParser parser, QString startchars, int prio )
{
	//base check
	if(parser==0 || startchars=="" || prio<0 || prio>100)
		return false;
	//search for existing entry
	for(int i=0;i<d->parsers.size();i++){
		if(d->parsers[i].parser==parser){
			d->parsers[i].start=startchars;
			d->parsers[i].prio=prio;
			return true;
		}
	}
	//none found: create new entry
	Private::LiteralParser_s s;
	s.parser=parser;
	s.prio=prio;
	s.start=startchars;
	d->parsers<<s;
	return true;
}

void Engine::removeLiteralParser ( ELAM::LiteralParser parser )
{
	for(int i=0;i<d->parsers.size();i++)
		if(d->parsers[i].parser==parser){
			d->parsers.removeAt(i);
			return;
		}
}

QPair< QString, QVariant > Engine::parseLiteral ( QString ex, int start)
{
	QChar sc=ex[start];
	//find any parser that matches the start char
	QList<Private::LiteralParser_s>cand;
	for(int i=0;i<d->parsers.size();i++)
		if(d->parsers[i].start.contains(sc))
			cand<<d->parsers[i];
	if(cand.size()<1)
		return QPair<QString,QVariant>();
	//sort them (highest prio first)
	qSort(cand);
	//execute
	for(int i=0;i<cand.size();i++){
		QPair< QString, QVariant >r=cand[i].parser(ex,*this,start);
		if(r.first.size()>0)
			return r;
	}
	//failure
	return QPair< QString, QVariant >();
}

BinaryOperator Engine::binaryOperator ( QString name,int prio,PriorityMatch match )
{
	//correct prio
	if(prio<1)prio=1;
	if(prio>99)prio=99;
	//if new: declare success
	if(!d->binary.contains(name)){
		d->binary.insert(name,Private::BinaryOperator_s(prio));
		return d->binary[name].op;
	}
	//check whether prio matches
	if(d->binary[name].prio==prio)
		return d->binary[name].op;
	//override if the op is empty
	if(d->binary[name].op.isNull())
		match=OverridePrio;
	//check matching mode
	switch(match){
		case OverridePrio:
			//change prio
			d->binary[name].prio=prio;
			//fall through
		case IgnoreMismatch:
			//return original op
			return d->binary[name].op;
			break;
		default://FailIfMismatch
			//return fake
			return BinaryOperator();
			break;
	}
}

int Engine::binaryOperatorPrio(QString name)
{
	if(d->binary.contains(name))
		return d->binary[name].prio;
	return -1;
}

void Engine::setBinaryOperatorPrio(QString name, int prio)
{
	binaryOperator(name,prio,OverridePrio);
}


UnaryOperator Engine::unaryOperator ( QString name )
{
	if(!d->unary.contains(name))
		d->unary.insert(name,UnaryOperator());
	return d->unary[name];
}

bool Engine::isAssignment(QString name) const
{
	//is it composed of operator chars?
	if(!d->cclass.isOperator(name))return false;
	//is there an overriding operator?
	if(d->binary.contains(name))return false;
	//return cclasses view of things
	return d->cclass.isAssignment(name);
}

bool Engine::hasBinaryOperator(QString name) const
{
	if(!d->binary.contains(name))return false;
	return ! d->binary[name].op.isNull();
}

bool Engine::hasUnaryOperator(QString name) const
{
	if(!d->unary.contains(name))return false;
	return ! d->unary[name].isNull();
}

void Engine::registerType(int typeId)
{
	if(!d->primtypes.contains(typeId))
		d->primtypes.append(typeId);
}

void Engine::setAutoCast(int target, QList< int > origin, TypeCast castfunc, int prio)
{
	if(castfunc==0)return;
	if(prio<0)return;
	registerType(target);
	Private::CastFunc_s cf(prio);
	cf.cfunc=castfunc;
	cf.origin=origin;
	cf.target=target;
	d->casts.append(cf);
}

QVariant Engine::autoCast(const QVariant& v)const
{
	//check for primary
	int vid=v.userType();
	if(d->primtypes.contains(vid))
		return v;
	//find matching cast
	int prio=-1;int pos=-1;
	for(int i=0;i<d->casts.size();i++){
		if(!d->casts[i].origin.contains(vid))continue;
		if(prio<d->casts[i].prio){
			pos=i;
			prio=d->casts[i].prio;
		}
	}
	//found it?
	if(pos<0)return v;
	else return d->casts[pos].cfunc(v);
}


//end of namespace
};