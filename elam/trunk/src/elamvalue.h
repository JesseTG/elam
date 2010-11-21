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
QDebug&operator<<(QDebug,const Position&);

/**Objects of this class represent an exception in the evaluation of an ELAM expression.*/
class Exception
{
	public:
		///Type of Exception
		enum ErrorType{
			///not an exception
			NoError=0,
			///error while parsing the expression
			ParserError,
			///the operator is not known
			UnknownOperatorError,
			///the function is not known
			UnknownFunctionError,
			///unknown variable or constant
			UnknownValueError,
			///the operator or function is known, but the type of one of the arguments is not supported
			TypeMismatchError,
			///the amount of arguments to a function is wrong or the syntax of arguments
			ArgumentListError,
			///the operation itself failed, e.g. division by zero
			OperationError,
		};
		
		///instantiates a copy of an exception
		Exception(const Exception&);
		///instantiate a copy from a matching variant
		Exception(const QVariant&);
		/**instantiates an exception
		\param type the type of exception
		\param errorText some human readable text describing the problem
		\param pos the position in the original expression string where the problem occurred*/
		Exception(ErrorType type=NoError,QString errorText=QString(),Position pos=Position());
		/**instantiates an exception
		\param type the type of exception
		\param errorText some human readable text describing the problem
		\param pos the position in the original expression string where the problem occurred*/
		Exception(ErrorType type,Position pos,QString errorText=QString());
		
		///copies an exception
		Exception& operator=(const Exception&);
		
		///returns the describing text of the exception (if existing)
		QString errorText()const{return merr;}
		///type of error represented by the exception
		ErrorType errorType()const{return mtype;}
		///the position in the original expression where the error originated
		Position errorPos()const{return mpos;}
		///the position in the original expression where the error originated
		Position position()const{return mpos;}
		///the line in the original expression where the error originated
		int errorLine()const{return mpos.line();}
		///the column in the original expression where the error originated
		int errorColumn()const{return mpos.column();}
		
		///overrides the exceptions position
		void setPosition(Position p){mpos=p;}
		
		///converts the exception to a variant
		operator QVariant()const{return QVariant::fromValue<Exception>(*this);}
		
		///the meta type ID (used with QVariant) of the exception type
		static int metaTypeId();
		
	private:
		ErrorType mtype;
		QString merr;
		Position mpos;
};

QDebug&operator<<(QDebug,const Exception&);

/**this type is not actually used, its ID is used as a fallback to tell operators, functions and engines that any supported type can be used*/
class AnyType
{
	public:
		///returns the meta type ID of the ANY type
		static int metaTypeId();
};
QDebug&operator<<(QDebug,const AnyType&);

//end of namespace
};


Q_DECLARE_METATYPE(ELAM::Exception);
Q_DECLARE_METATYPE(ELAM::AnyType);

#endif
