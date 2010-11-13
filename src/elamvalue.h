//ELAM value definition header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_VALUE_H
#define ELAM_VALUE_H

#include <QVariant>
#include <QString>
#include <QPair>
#include <QPoint>

namespace ELAM {

class Position
{
	public:
		Position(const QPair<int,int>&p){mline=p.first;mcol=p.second;}
		Position(const QPoint &p){mline=p.y();mcol=p.x();}
		Position(int line,int col){mline=line;mcol=col;}
		Position(int col){mline=1;mcol=col;}
		Position(){mline=mcol=-1;}
		
		int line()const{return mline;}
		int column()const{return mcol;}
		
		operator QPair<int,int>()const{return QPair<int,int>(mline,mcol);}
		operator QPoint()const{return QPoint(mcol,mline);}
	private:
		int mline,mcol;
};

/**Objects of this class represent an exception in the evaluation of an ELAM expression.*/
class Exception
{
	public:
		enum ErrorType{
			NoError=0,
			ParserError,
			UnknownOperatorError,
			UnknownFunctionError,
			TypeMismatchError,
		};
		
		Exception();
		Exception(const Exception&);
		Exception(ErrorType type,QString errorText=QString(),Position p=Position());
		
		Exception& operator=(const Exception&);
		
		QString errorText()const{return merr;}
		int errorLine()const{return mpos.line();}
		int errorColumn()const{return mpos.column();}
		Position errorPos()const{return mpos;}
		
		static int metaTypeId();
		
	private:
		ErrorType mtype;
		QString merr;
		Position mpos;
};

/**this type is not actually used, its ID is used as a fallback to tell operators, functions and engines that any supported type can be used*/
class AnyType
{
	public:
		///returns the meta type ID of the ANY type
		static int metaTypeId();
};

//end of namespace
};

Q_DECLARE_METATYPE(ELAM::Exception);
Q_DECLARE_METATYPE(ELAM::AnyType);

#endif
