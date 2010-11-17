//  engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamengine.h"
#include "elamvalue.h"

#include <QDebug>

namespace ELAM {

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


//end of namespace
};