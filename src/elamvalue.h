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

/** \brief A character position inside a text/string that is being evaluated.

A position consists of line and column. The top left position in a text is assumed to be line 1 column 1. Any position with line and/or column less than zero is considered invalid.

This class is completely inline and as such should be quite fast.*/
class Position
{
	public:
		///converts a pair of ints to a position
		inline Position(const QPair<int,int>&p){mline=p.first;mcol=p.second;}
		///converts a QPoint to a position - x is interpreted as column, y as line
		inline Position(const QPoint &p){mline=p.y();mcol=p.x();}
		///instantiates a position from line and column
		inline Position(int line,int col){mline=line;mcol=col;}
		///instantiates a position from column only, line is assumed to be "1"
		inline Position(int col){mline=1;mcol=col;}
		///instantiates an invalid position
		inline Position(){mline=mcol=-1;}
		///copy constructor
		inline Position(const Position&p){mline=p.mline;mcol=p.mcol;}
		
		///copies a position
		inline Position& operator=(const Position&p){mline=p.mline;mcol=p.mcol;return *this;}
		
		///returns the line of this position or -1 if the position is invalid
		inline int line()const{if(mcol>=0)return mline;else return -1;}
		///returns the column of this position or -1 if the position is invalid
		inline int column()const{if(mline>=0)return mcol;else return -1;}
		
		///true if this position is valid
		inline bool isValid()const{return mline>=0 && mcol >=0;}
		
		///transparently converts to a pair of ints
		inline operator QPair<int,int>()const{return QPair<int,int>(mline,mcol);}
		///transparently converts to a QPoint, with x=column and y=line
		inline operator QPoint()const{return QPoint(mcol,mline);}
	private:
		int mline,mcol;
};
QDebug&operator<<(QDebug&,const Position&);

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
