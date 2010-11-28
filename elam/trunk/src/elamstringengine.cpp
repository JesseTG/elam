// ELAM int engine definition implementation
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#include "elamstringengine.h"

#include<QDebug>

using namespace ELAM;

StringEngine::StringEngine()
{
	configureStringEngine(*this);
}

static QString unescape(QString ls)
{
	QString r,cd;
	enum {Normal,BSlash,Octal,Hexa}mode=Normal;
	for(int i=0;i<ls.size();i++){
		if(mode==BSlash){
			switch(ls[i].unicode()){
				case '\\':r+='\\';break;
				case 'n':r+='\n';break;
				case 'r':r+='\r';break;
				case 't':r+='\t';break;
				case 'v':r+='\v';break;
				case 'b':r+='\b';break;
				case 'f':r+='\f';break;
				case 'a':r+='\a';break;
				case '\'':r+='\'';break;
				case '\"':r+='\"';break;
				case '0':case '1':case '2':
				case '3':case '4':case '5':
				case '6':case '7':
					mode=Octal;
					cd=ls[i];
					break;
				case 'x':
					mode=Hexa;
					cd.clear();
					break;
				case 'u':
					cd=ls.mid(i+1,4); i+=4;
					r+=QChar(cd.toInt(0,16));
					mode=Normal;
					break;
				case 'U':
					cd=ls.mid(i+1,8); i+=8;
					r+=QChar(cd.toInt(0,16));
					mode=Normal;
					break;
				default:
					r+=ls[i];
					break;
			}
			if(mode==BSlash)mode=Normal;
		}else if(mode==Octal){
			if(ls[i]>='0' && ls[i]<='7')
				cd+=ls[i];
			else mode=Normal;
			if(cd.size()>=3)
				mode=Normal;
			if(mode==Normal)
				r+=QChar(cd.toInt(0,8));
		}else if(mode==Hexa){
			if((ls[i]>='0' && ls[i]<='9') ||
			   (ls[i]>='a'&&ls[i]<='f') ||
			   (ls[i]>='A'&&ls[i]<='F'))
				cd+=ls[i];
			else mode=Normal;
			if(cd.size()>=2)
				mode=Normal;
			if(mode==Normal)
				r+=QChar(cd.toInt(0,16));
		}else{
			if(ls[i]!='\\')r+=ls[i];
			else mode=BSlash;
		}
	}
	return r;
}

//types of int
// decimal: [1-9][0-9]*
// octal: 0[0-7]*
// hex: 0x[0-9a-fA-F]+
// end of expression: anything not a nameClass.second
static QPair<QString,QVariant> strLiteralParser(const QString&expr,Engine&engine,int start)
{
	Q_UNUSED(engine);
	QString ls;
	QChar startc=expr[start];
	QChar prev='\\';
	//parse to end of expression
	bool foundend=false;
	for(int i=start;i<expr.size();i++){
		ls+=expr[i];
		if(expr[i]==startc && prev!='\\'){
			foundend=true;
			break;
		}
		prev=expr[i];
	}
	//found it?
	if(!foundend)
		return QPair<QString,QVariant>();
	//convert
	QString r=unescape(ls.mid(1,ls.size()-2));
	//return result
	return QPair<QString,QVariant>(ls,r);
}

static QVariant strFunc(const QList<QVariant>&lf)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	if(!lf[0].canConvert<QString>())
		return Exception(Exception::TypeMismatchError,"cannot convert to string");
	return lf[0].toString();
}

static QVariant strlenFunc(const QList<QVariant>&lf)
{
	if(lf.size()!=1)
		return Exception(Exception::ArgumentListError, "expecting exactly one argument");
	if(!lf[0].canConvert<QString>())
		return Exception(Exception::TypeMismatchError,"cannot convert to string");
	return (qlonglong)lf[0].toString().size();
}

static QVariant concatFunc(const QList<QVariant>&lf)
{
	QString ret;
	for(int i=0;i<lf.size();i++)
		ret+=lf[i].toString();
	return ret;
}
//additive
static QVariant strAdd(const QVariant&o1,const QVariant&o2)
{
	return o1.toString()+o2.toString();
}

int StringEngine::stringParserPrio()
{
	return 50;
}

static QVariant strCast(const QVariant&v){return v.toString();}

void StringEngine::configureStringEngine(Engine& eng)
{
	int sid=QVariant::String;
	int aid=AnyType::metaTypeId();
	eng.setAutoCast(sid, QList<int>()<<QVariant::Char, strCast, 40);
	//parser
	eng.setLiteralParser(strLiteralParser,"\'\"",stringParserPrio());
	//cast
	eng.setFunction("string",strFunc);
	//other functions
	eng.setFunction("strlen",strlenFunc);
	eng.setFunction("concat",concatFunc);
	//operators
	eng.binaryOperator("+",80).setCallback(strAdd,sid,sid);
	eng.binaryOperator("+",80).setCallback(strAdd,aid,sid);
	eng.binaryOperator("+",80).setCallback(strAdd,sid,aid);
}
