//  engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamengine.h"
#include "elamvalue.h"

namespace ELAM {
///////////////////////////////////////////////////////////////////////////////
// Engine

class DPTR_CLASS_NAME(Engine):public DPtr
{
	public:
		CharacterClassSettings cclass;
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
	Token::Type ctype=Token::Invalid;
	QString ctok;
	QList<Token> ret;
	int cline=1,ccol=0;
	int sline=cline,scol=ccol;
	for(int i=0;i<ex.size();i++){
		if(ex[i]=='\n'){cline++;ccol=0;}
		ccol++;
		//if we have a type: check that we are still in it
		Token::Type ntype=d->cclass.charType(ex[i],ctype);
		//check for invalid stuff
		if(ntype==Token::Invalid)
			return QList<Token>()<<Token(Position(sline,scol));
		//check whether we are still inside the same token
		if(ntype==ctype)
			ctok+=ex[i];
		else{
			if(ctype!=Token::Invalid)
				ret<<Token(ctok,ctype,Position(sline,scol));
			ctype=ntype;
			ctok.clear();
			sline=cline;
			scol=ccol;
		}
	}
	return ret;
}

CharacterClassSettings Engine::characterClasses()
{
	return d->cclass;
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
	//check remaining special chars are not in any other class
	any+=d->operatorClass;
	if(any.contains(d->parenthesesChars.first) ||
	   any.contains(d->parenthesesChars.second) ||
	   any.contains(d->commaChar))
		return false;
	//all ok
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