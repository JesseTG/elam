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
};
DEFINE_DPTR(Engine);

Engine::Engine(QObject* parent): QObject(parent)
{

}

QVariant Engine::evaluate(QString ex)
{
	return evaluate(expression(ex));
}

QVariant Engine::evaluate(Expression ex)
{
	Q_UNUSED(ex);
	//TODO: implement
	return QVariant();
}

Expression Engine::expression(QString s)
{
	return Expression(tokenize(s));
}

Expression Engine::expression(QList< Token > )
{
	//TODO: implement parser
	return Expression();
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
	d->consts.insert(n,v);
	return true;
}

bool Engine::setVariable(QString n, QVariant v)
{
	if(!d->cclass.isName(n))return false;
	if(d->consts.contains(n))return false;
	if(d->funcs.contains(n))return false;
	d->vars.insert(n,v);
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
	if(parser==0 || startchars=="" || prio<0 || prio>100)
		return false;
	for(int i=0;i<d->parsers.size();i++){
		if(d->parsers[i].parser==parser){
			d->parsers[i].start=startchars;
			d->parsers[i].prio=prio;
			return true;
		}
	}
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
		QPair< QString, QVariant >r=cand[0].parser(ex,*this,start);
		if(r.first.size()>0)
			return r;
	}
	//failure
	return QPair< QString, QVariant >();
}


/////////////////////////////////////////////////////////////////
// character classes

class DPTR_CLASS_NAME(CharacterClassSettings):public SharedDPtr
{
	public:
		DPTR_NAME(){
			operatorClass= "~!@#$%^&*-+=:<>?/";
			literalClass="0123456789\'\"";
			nameClass= QPair<QString,QString>( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_",  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
			whitespaceClass=" \t\r\n\v";
			parenthesesChars=QPair<QChar,QChar>('(',')');
			assignmentChars=QPair<QChar,QChar>(0,'=');
			commaChar=',';
		}
		QString operatorClass;
		QString literalClass;
		QPair<QString,QString> nameClass;
		QString whitespaceClass;
		QPair<QChar,QChar> parenthesesChars;
		QPair<QChar,QChar> assignmentChars;
		QChar commaChar;
};
DEFINE_SHARED_DPTR(CharacterClassSettings);

QString CharacterClassSettings::operatorClass() const
{
	return d->operatorClass;
}
void CharacterClassSettings::setOperatorClass(QString c)
{
	d->operatorClass=c;
}

QPair<QString,QString> CharacterClassSettings::nameClass() const
{
	return d->nameClass;
}
void CharacterClassSettings::setNameClass(QString f,QString s)
{
	d->nameClass.first=f;
	d->nameClass.second=s;
}

QString CharacterClassSettings::whitespaceClass() const
{
	return d->whitespaceClass;
}
void CharacterClassSettings::setWhitespaceClass(QString c)
{
	d->whitespaceClass=c;
}

QPair< QChar, QChar > CharacterClassSettings::assignmentChars() const
{
	return d->assignmentChars;
}
void CharacterClassSettings::setAssignmentChars(QChar f, QChar s)
{
	d->assignmentChars.first=f;
	d->assignmentChars.second=s;
}

QChar CharacterClassSettings::commaChar() const
{
	return d->commaChar;
}
void CharacterClassSettings::setCommaChar(QChar c)
{
	d->commaChar=c;
}

QPair< QChar, QChar > CharacterClassSettings::parenthesesChars() const
{
	return d->parenthesesChars;
}
void CharacterClassSettings::setParentheses(QChar f, QChar s)
{
	d->parenthesesChars.first=f;
	d->parenthesesChars.second=s;
}

QString CharacterClassSettings::literalStartClass() const
{
	return d->literalClass;
}
void CharacterClassSettings::setLiteralStartClass(QString c)
{
	d->literalClass=c;
}

static bool strContainedIn(QString n,QString h)
{
	for(int i=0;i<n.size();i++)
		if(!h.contains(n[i]))
			return false;
	return true;
}

static bool strOverlaps(QString s1,QString s2)
{
	for(int i=0;i<s1.size();i++)
		if(s2.contains(s1[i]))
			return true;
	return false;
}

bool CharacterClassSettings::isConsistent() const
{
	//check that check name class is internally consistent
	if(!strContainedIn(d->nameClass.first,d->nameClass.second))
		return false;
	//check that major classes do not overlap
	//literals: operators, whitespace, start of names (may partially overlap with names)
	if(strOverlaps(d->literalClass,d->operatorClass+d->whitespaceClass+d->nameClass.first))
		return false;
	//operators: whitespace, literals, all of names
	if(strOverlaps(d->operatorClass,d->literalClass+d->whitespaceClass+d->nameClass.second))
		return false;
	//start of names: literals, operators, whitespace
	if(strOverlaps(d->nameClass.first,d->operatorClass+d->whitespaceClass+d->literalClass))
		return false;
	//all of names: operators, whitespace
	if(strOverlaps(d->nameClass.second,d->operatorClass+d->whitespaceClass))
		return false;
	//check assigment chars are not in any class (except operators)
	QString any=d->literalClass+d->whitespaceClass+d->nameClass.second+d->literalClass;
	if(any.contains(d->assignmentChars.first) || any.contains(d->assignmentChars.second))
		return false;
	if(d->assignmentChars.first!=0 && !d->operatorClass.contains(d->assignmentChars.first))
		return false;
	if(d->assignmentChars.second!=0 && !d->operatorClass.contains(d->assignmentChars.second))
		return false;
	//check parentheses are different
	if(d->parenthesesChars.first==d->parenthesesChars.second)
		return false;
	//check remaining special chars are not in any other class
	any+=d->operatorClass;
	if(any.contains(d->parenthesesChars.first) ||
	   any.contains(d->parenthesesChars.second) ||
	   any.contains(d->commaChar))
		return false;
	//all ok
	return true;
}

Token::Type CharacterClassSettings::charType(QChar c, ELAM::Token::Type otype) const
{
	//special char?
	if(c==d->parenthesesChars.first)return Token::ParOpen;
	if(c==d->parenthesesChars.second)return Token::ParClose;
	if(c==d->commaChar)return Token::Comma;
	//is it a name?
	if(otype==Token::Name){
		//is this a continuation
		if(d->nameClass.second.contains(c))return Token::Name;
	}else{
		//is this the start of a name
		if(d->nameClass.first.contains(c))return Token::Name;
	}
	//is it the start of a literal?
	if(d->literalClass.contains(c))return Token::Literal;
	//operator?
	if(d->operatorClass.contains(c))return Token::Operator;
	//whitespace?
	if(d->whitespaceClass.contains(c))return Token::Whitespace;
	//must be invalid noise
	return Token::Invalid;
}

bool CharacterClassSettings::isAssignment(QString op)const
{
	//sanity checks: size
	if(op.size()<1)return false;
	if(d->assignmentChars.first!=0 && d->assignmentChars.second!=0)
		if(op.size()<2)return false;
	//check we have assignments at all
	if(d->assignmentChars.first==0 && d->assignmentChars.second==0)return false;
	//check it is assignment
	if(d->assignmentChars.first!=0 && op[0]!=d->assignmentChars.first)
		return false;
	if(d->assignmentChars.second!=0 && op[op.size()-1]!=d->assignmentChars.second)
		return false;
	//check it is an operator
	for(int i=0;i<op.size();i++)
		if(!d->operatorClass.contains(op[i]))
			return false;
	//passed everything
	return true;
}

bool CharacterClassSettings::isSimpleAssignment ( QString op) const
{
	if(op.size()<1)return false;
	QString c;
	if(d->assignmentChars.first!=0)c+=d->assignmentChars.first;
	if(d->assignmentChars.second!=0)c+=d->assignmentChars.second;
	return op==c;
}


bool CharacterClassSettings::isName(QString n) const
{
	if(n.size()<1)return false;
	if(!d->nameClass.first.contains(n[0]))return false;
	for(int i=0;i<n.size();i++)
		if(!d->nameClass.second.contains(n[i]))return false;
	return true;
}

bool CharacterClassSettings::isOperator(QString op) const
{
	for(int i=0;i<op.size();i++)
		if(!d->operatorClass.contains(op[i]))
			return false;
	return true;
}


/////////////////////////////////////////////////////////////////
// Unary Operator

class DPTR_CLASS_NAME(UnaryOperator):public SharedDPtr
{
	public:
		QMap<int,UnaryOperatorCall>callmap;
};
DEFINE_SHARED_DPTR(UnaryOperator)


UnaryOperator::UnaryOperator(const UnaryOperator& op)
	:d(op.d)
{
}

UnaryOperator& UnaryOperator::UnaryOperator::operator=(const ELAM::UnaryOperator& op)
{
	d=op.d;
	return *this;
}


UnaryOperator::UnaryOperator()
{
}

UnaryOperator::~UnaryOperator()
{
}


QVariant UnaryOperator::execute(const QVariant& op) const
{
	if(d->callmap.size()==0)
		return qVariantFromValue<Exception>(Exception(Exception::UnknownOperatorError));
	//search for direct match
	if(d->callmap.contains(op.type()))
		return d->callmap[op.type()](op);
	//search for fallback
	int any=AnyType::metaTypeId();
	if(d->callmap.contains(any))
		return d->callmap[any](op);
	return qVariantFromValue<Exception>(Exception(Exception::TypeMismatchError));
}

QList< int > UnaryOperator::getTypeIds() const
{
	return d->callmap.keys();
}

QStringList UnaryOperator::getTypeNames() const
{
	QStringList ret;
	QList<int>ti=d->callmap.keys();
	for(int i=0;i<ti.size();i++)
		ret<<QVariant::typeToName((QVariant::Type)ti[i]);
	return ret;
}

UnaryOperatorCall UnaryOperator::getCallback(QString type) const
{
	return getCallback(QVariant::nameToType(type.toAscii().data()));
}

UnaryOperatorCall UnaryOperator::getCallback(int type) const
{
	if(d->callmap.contains(type))
		return d->callmap[type];
	if(d->callmap.contains(AnyType::metaTypeId()))
		return d->callmap[AnyType::metaTypeId()];
	return 0;
}

void UnaryOperator::setCallback(UnaryOperatorCall callback, int type)
{
	if(type==QVariant::Invalid)return;
	if(callback==0){
		d->callmap.remove(type);
		return;
	}
	d->callmap.insert(type,callback);
}

void UnaryOperator::setCallback(UnaryOperatorCall callback, QString type)
{
	setCallback(callback,QVariant::nameToType(type.toAscii().data()));
}

void UnaryOperator::removeCallback(UnaryOperatorCall cb)
{
	if(cb==0)return;
	QList<int>k=d->callmap.keys(cb);
	for(int i=0;i<k.size();i++)
		d->callmap.remove(k[i]);
}

void UnaryOperator::removeCallback(QString t)
{
	removeCallback(QVariant::nameToType(t.toAscii().data()));
}

void UnaryOperator::removeCallback(int t)
{
	d->callmap.remove(t);
}

/////////////////////////////////////////////////////////////////
// Binary Operator

class DPTR_CLASS_NAME(BinaryOperator):public SharedDPtr
{
	public:
		QMap<QPair<int,int>,BinaryOperatorCall>callmap;
};
DEFINE_SHARED_DPTR(BinaryOperator)


BinaryOperator::BinaryOperator(const BinaryOperator& op)
	:d(op.d)
{
}

BinaryOperator::BinaryOperator()
{

}

BinaryOperator::~BinaryOperator()
{

}


//end of namespace
};