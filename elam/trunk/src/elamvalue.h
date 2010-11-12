//ELAM value definition header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_VALUE_H
#define ELAM_VALUE_H

#include <QVariant>
#include <QString>
#include <QPair>

/**Objects of this class represent an exception in the evaluation of an ELAM expression.*/
class ELAMException
{
	public:
		enum ErrorType{
			NoError=0,
			ParserError,
			UnknownOperatorError,
			UnknownFunctionError,
			TypeMismatchError,
		};
		
		ELAMException();
		ELAMException(const ELAMException&);
		ELAMException(ErrorType type,QString errorText=QString(),int line=-1,int column=-1);
		
		ELAMException& operator=(const ELAMException&);
		
		QString errorText()const{return merr;}
		int errorLine()const{return mline;}
		int errorColumn()const{return mcol;}
		QPair<int,int> errorPos()const{return QPair<int,int>(mline,mcol);}
		
		static int metaTypeId();
		
	private:
		ErrorType mtype;
		QString merr;
		int mline,mcol;
};
Q_DECLARE_METATYPE(ELAMException);

/**this type is not actually used, its ID is used as a fallback to tell operators, functions and engines that any supported type can be used*/
class ELAMAnyType
{
	public:
		///returns the meta type ID of the ANY type
		static int metaTypeId();
};
Q_DECLARE_METATYPE(ELAMAnyType);

#endif
