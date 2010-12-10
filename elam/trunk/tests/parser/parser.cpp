#include "elam.h"

#include "parser.h"

#include <QtCore>
#include <QtTest>
// #include <QDebug>

using namespace ELAM;

void ElamTest::charClass()
{
	CharacterClassSettings def;
	//consistency of default
	QCOMPARE(def.isConsistent(),true);
	//name class
	QCOMPARE(def.isName("halloWelt0_x"),true);
	QCOMPARE(def.isName("_halloWelt0"),true);
	QCOMPARE(def.isName("0halloWelt0_x"),false);
	//operator class
	QCOMPARE(def.isOperator("=*+-"),true);
	QCOMPARE(def.isOperator("a-+"),false);
	//right side assigment
	QCOMPARE(def.isAssignment("="),true);
	QCOMPARE(def.isAssignment("+="),true);
	QCOMPARE(def.isAssignment("=+"),false);
	QCOMPARE(def.isSimpleAssignment("="),true);
	QCOMPARE(def.isSimpleAssignment("+="),false);
	//left side assigment
	def.setAssignmentChars('=',0);
	QCOMPARE(def.isAssignment("="),true);
	QCOMPARE(def.isAssignment("=+"),true);
	QCOMPARE(def.isAssignment("+="),false);
	QCOMPARE(def.isSimpleAssignment("="),true);
	QCOMPARE(def.isSimpleAssignment("=+"),false);
	//two sided assigment
	def.setAssignmentChars(':','=');
	QCOMPARE(def.isAssignment(":="),true);
	QCOMPARE(def.isAssignment("="),false);
	QCOMPARE(def.isAssignment(":"),false);
	QCOMPARE(def.isAssignment(":+="),true);
	QCOMPARE(def.isAssignment(":=+"),false);
	QCOMPARE(def.isAssignment("+:="),false);
	QCOMPARE(def.isSimpleAssignment(":="),true);
	QCOMPARE(def.isSimpleAssignment(":+="),false);
	QCOMPARE(def.isSimpleAssignment(":"),false);
	QCOMPARE(def.isSimpleAssignment("="),false);
	//collision between name/operator
	def.setOperatorClass("+-abcd");
	QCOMPARE(def.isConsistent(),false);
	//collision between name/literal
	def=CharacterClassSettings();
	def.setLiteralStartClass("abcd");
	QCOMPARE(def.isConsistent(),false);
	//collision between literal/operator
	def.setLiteralStartClass("+-");
	QCOMPARE(def.isConsistent(),false);
	//collision between parentheses and names/ops/literals/itself
	def=CharacterClassSettings();
	def.setParentheses('a','b');
	QCOMPARE(def.isConsistent(),false);
	def.setParentheses('+','-');
	QCOMPARE(def.isConsistent(),false);
	def.setParentheses('0','1');
	QCOMPARE(def.isConsistent(),false);
	def.setParentheses('(','(');
	QCOMPARE(def.isConsistent(),false);
}

void ElamTest::tokenizer()
{
	IntEngine ie;
	FloatEngine::configureFloatEngine(ie);
	QString ex="a= bcd +345*efg*(65.3/(5))";
	QList<Token> tl=ie.tokenize(ex);
	/*qDebug()<<"expression:"<<ex<<"tokens:"<<tl.size();
	for(int i=0;i<tl.size();i++)
		qDebug()<<" token"<<i<<tl[i];*/
	QCOMPARE(tl.size(),15);
	QCOMPARE(tl[0].type(),Token::Name);//a
	QCOMPARE(tl[1].type(),Token::Operator);//=
	QCOMPARE(tl[2].type(),Token::Name);//bcd
	QCOMPARE(tl[3].type(),Token::Operator);//+
	QCOMPARE(tl[4].type(),Token::Literal);//345
	QCOMPARE(tl[4].literalValue().toInt(),345);//345
	QCOMPARE(tl[4].literalValue().typeName(),"qlonglong");//345
	QCOMPARE(tl[4].literalValue().type(),QVariant::LongLong);//345
	QCOMPARE(tl[5].type(),Token::Operator);//*
	QCOMPARE(tl[6].type(),Token::Name);//efg
	QCOMPARE(tl[7].type(),Token::Operator);//*
	QCOMPARE(tl[8].type(),Token::ParOpen);//(
	QCOMPARE(tl[9].type(),Token::Literal);//65.3
	QCOMPARE(tl[9].literalValue().type(),QVariant::Double);
	QCOMPARE(tl[10].type(),Token::Operator);// /
	QCOMPARE(tl[11].type(),Token::ParOpen);//(
	QCOMPARE(tl[12].type(),Token::Literal);//5
	QCOMPARE(tl[13].type(),Token::ParClose);//)
	QCOMPARE(tl[14].type(),Token::ParClose);//)
}

void ElamTest::stringLiteral()
{
	StringEngine se;
	IntEngine::configureIntEngine(se);
	QList<Token>tl=se.tokenize("\"\\0123\\u12abcd\"");
	QString v=QString("\012")+"3"+QChar(0x12ab)+QString("cd");
	QCOMPARE(tl.size(),1);
	QCOMPARE(tl[0].isLiteral(),true);
	QString l=tl[0].literalValue().toString();
// 	qDebug()<<"v="<<v<<"l="<<l;
	QCOMPARE(v,l);
	QCOMPARE(se.evaluate("strlen(\"abc\")").toInt(),3);
	QVariant v2=se.evaluate("concat(\"abc\",12)");
	QCOMPARE(v2.toString(),QString("abc12"));
}

void ElamTest::emptyList()
{
	IntEngine ie;
	QList<Token>tl=ie.tokenize("");
	QCOMPARE(tl.size(),0);
	tl=ie.tokenize(" \t\n");
	QCOMPARE(tl.size(),0);
}

QTEST_MAIN(ElamTest)