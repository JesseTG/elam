// main header
//
// (c) Konrad Rosenbaum, 2010
// protected under the GNU LGPL v3 or at your option any newer

#ifndef ELAM_ENGINE_CCLASS_H
#define ELAM_ENGINE_CCLASS_H

#include <QStringList>
#include <QObject>
#include <QVariant>

#include "elamexpression.h"

#include "dptr.h"

namespace ELAM {

/** \brief This class holds the character classes used by an Engine.

\see Engine::characterClasses()

There are three major classes of chracters:
 - names
  - names can be variables, constants, functions
  - the names class has two sub-classes
   - start-of-names is characters that may start a name, usually letters and underscores
   - all-of-names is characters that may be contained anywhere in names, this class must include the complete start of names class
 - operators
  - any sequence of these characters is interpreted as unary or binary operator
 - whitespace
  - whitespace can separate other tokens, but is ignored itself

None of these three classes may overlap. There are several more minor classes:
 - literals
  - the character class actually only contains characters that can start a literal, the continuation and end of literals is determines by a callback routine
  - characters in this class may overlap with names, but must not overlap with operators, whitespace and start-of-names
 - parentheses
  - are two distinct characters that group expressions, none of them must be contained in any other class
 - comma
  - a single character separating arguments in functions, the comma character must not be contained in any other class
 - assignment
  - one or two characters that denote an assignment
  - both are optional, both must be contained in operators
  - one of the characters, if it exists, is the first character of an assignment
  - the other of the characters, if it exists, is the last character of an assignment
  - just those two characters are the simple assignment operator
  - both surrounding another operator combine the operation with assignment (e.g. a+=b is equivalent to a=a+b)

*/
class CharacterClassSettings
{
	DECLARE_SHARED_DPTR(d)
	public:
		///class of operator characters
		QString operatorClass()const;
		///sets the class of operator characters
		void setOperatorClass(QString);
		
		/**returns the two name sub-classes
		nameClass().first refers to start characters, 
		nameClass().second refers to all characters that can turn up anywhere in names*/
		QPair<QString,QString> nameClass()const;
		/**sets the two sub-classes of names
		\param startchars are characters that can start a name
		\param allchars are characters that can turn up anywhere in names, allchars must include all characters from startchars*/
		void setNameClass(QString startchars,QString allchars);
		
		///returns all characters that are regarded as whitespace
		QString whitespaceClass()const;
		///sets all characters that are regarded as whitespace
		void setWhitespaceClass(QString);
		
		///returns all characters that start a literal, normally digits ' and "
		QString literalStartClass()const;
		///sets characters that can start a literal
		void setLiteralStartClass(QString);
		
		///returns the opening (first) and closing (second) character of parentheses
		QPair<QChar,QChar> parenthesesChars()const;
		/**sets the characters used for parentheses
		\param open the character that opens/begins a parentheses structure, normally "("
		\param close the character that closes/ends a parentheses structure, normally ")"
		
		The parentheses characters must not be included in any other class.*/
		void setParentheses(QChar open,QChar close);
		
		/** \brief returns the characters that designate an assignment
		
		- assignmentChars().first is the character that starts an assignment operator,
		- assignmentChars().second is the character that ends it.*/
		QPair<QChar,QChar> assignmentChars()const;
		
		/** sets the characters used for assignment operators
		\param start if not '\0' the character that marks the start of an assignment
		\param end if not '\0' the character that marks the end of an assignment
		
		The combination of both characters along (without whitespace) is the direct assignment operator. In automatic assignment operator mode any operator that starts with the start character and ends with end character is regarded as an implicit assignment.
		
		In the default configuration the start character is not set ('\0') and the end character is '=', so "a=1" will assign the value "1" to the variable "a" and "a += 1" is equivalent to "a = a + 1".
		
		You can turn this automatism around by defining a start character only (e.g. start='=' and end='\0'). Then the assignment would still be "a = 1", but the combination of assignment and "+" would become "a =+ 1".
		
		If you define both characters then both must be present in assignments. For example with start=':' and end='=' then assignment becomes "a:=1" and assignment with "+" becomes "a :+= 1".
		
		If set, both characters must be part of the operator class.
		
		If both start and end are '\0' it will be impossible to make assignments*/
		void setAssignmentChars(QChar start,QChar end);
		
		///returns the character used as a comma (separator of function arguments)
		QChar commaChar()const;
		/**sets the character used as comma
		
		The character must not be part of any other class.*/
		void setCommaChar(QChar);
		
		/**true if the settings are internally consistent
		
		The character class settings are consistent if all constraints of class inclusion and exclusion are fullfilled.*/
		bool isConsistent()const;
		
		/**returns the type of token the character belongs to
		\param ch the character to be checked
		\param oldtype the type the previous character belongs to (set to Invalid if this is the first character)
		
		Compares the character with the known settings for character classes and returns its probable token type. The old type of the previous character is necessary to check for context sensitive rules.
		
		\returns the type of this character:
		 - Invalid is returned if the character does not match any class, parsing should stop here
		 - If Literal is returned the calling engine must use specialized literal checking functions to find the end of the literal, the next character checke with this routine should be the first one behind the end of the literal
		 - Assignment characters are returned as Operator class
		 - Whitespace class characters should be ignored
		 - Special character types (Par*, Comma) must be tokenized separately
		 - Any other type (Name, Operator) must be concatenated until the return type changes
		*/
		Token::Type charType(QChar ch,Token::Type oldtype)const;
		
		///returns true if the string contains any assignment operator
		bool isAssignment(QString)const;
		///returns true if the string contains exactly the simple assignment operator
		bool isSimpleAssignment(QString)const;
		
		///returns true if the string can be interpreted as name
		bool isName(QString)const;
		
		///returns true if the string can be interpreted as operator
		bool isOperator(QString)const;
		
		///removes assignment chars from the token and returns the pure operator or an empty string if it was a pure assignment
		QString toOperator(QString)const;
};

//end of namespace
};

#endif
