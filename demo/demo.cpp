#include "elam.h"
#include "demo.h"

#include <QApplication>
#include <QTabWidget>
#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTreeView>
#include <QStandardItemModel>
#include <QHeaderView>

//central engine instance
ELAM::Engine engine;

ElamDemo::ElamDemo(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
	//basics
	setWindowTitle("ELAM Demo");
	setSizeGripEnabled(true);
	resize(600,400);
	QVBoxLayout*vl;
	QHBoxLayout*hl;
	setLayout(vl=new QVBoxLayout);
	vl->addWidget(mtabs=new QTabWidget);
	QWidget *w;
	QPushButton*p;
	mtabs->addTab(w=new QWidget,"Expression");
	//expression line
	w->setLayout(vl=new QVBoxLayout);
	vl->addLayout(hl=new QHBoxLayout);
	hl->addWidget(mexpression=new QLineEdit);
	hl->addWidget(p=new QPushButton("exec"));
	connect(p,SIGNAL(clicked()),this,SLOT(execute()));
	//result label
	vl->addWidget(mresult=new QLabel);
	
	//variables/constants table
	mtabs->addTab(mvalues=new QTreeView,"Values");
	mvalues->setModel(mvalmodel=new QStandardItemModel(this));
	mvalues->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	//functions/operators table
	mtabs->addTab(msyntax=new QTreeView,"Syntax");
	msyntax->setModel(msynmodel=new QStandardItemModel(this));
	msyntax->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	//initialize
	updateViews();
}

void ElamDemo::execute()
{
	//evaluate expression
	QVariant res=engine.evaluate(mexpression->text());
	//get result as printable string
	QString str;
	int type=res.userType();
	//handle Exception specially
	if(type==ELAM::Exception::metaTypeId()){
		ELAM::Exception ex=res.value<ELAM::Exception>();
		str=QString("Exception at %1:%2 - %3")
			.arg(ex.errorLine())
			.arg(ex.errorColumn())
			.arg(ex.errorText());
	}else{
		str=QString("Type %1, Value: %2")
			.arg(res.typeName())
			.arg(res.toString());
	}
	mresult->setText(str);
	//update tables (in case of assignments)
	updateViews();
}

void ElamDemo::updateViews()
{
	mvalmodel->clear();
	mvalmodel->insertRows(0,2);
	if(mvalmodel->columnCount()<3)
		mvalmodel->insertColumns(0,3);
	mvalmodel->setHorizontalHeaderLabels(QStringList()<<"Name"<<"Type"<<"Value");
	//constants
	QModelIndex pidx=mvalmodel->index(0,0);
	mvalmodel->setData(pidx,"Constants");
	QStringList k=engine.constantNames();
	mvalmodel->insertRows(0,k.size(),pidx);
	mvalmodel->insertColumns(0,3,pidx);
	for(int i=0;i<k.size();i++){
		mvalmodel->setData(mvalmodel->index(i,0,pidx),k[i]);
		mvalmodel->setData(mvalmodel->index(i,1,pidx),engine.getConstant(k[i]).typeName());
		mvalmodel->setData(mvalmodel->index(i,2,pidx),engine.getConstant(k[i]).toString());
	}
	//variables
	k=engine.variableNames();
	pidx=mvalmodel->index(1,0);
	mvalmodel->setData(pidx,"Variables");
	mvalmodel->insertRows(0,k.size(),pidx);
	mvalmodel->insertColumns(0,3,pidx);
	for(int i=0;i<k.size();i++){
		mvalmodel->setData(mvalmodel->index(i,0,pidx),k[i]);
		mvalmodel->setData(mvalmodel->index(i,1,pidx),engine.getVariable(k[i]).typeName());
		mvalmodel->setData(mvalmodel->index(i,2,pidx),engine.getVariable(k[i]).toString());
	}
	mvalues->expandAll();
	mvalues->resizeColumnToContents(0);
	mvalues->resizeColumnToContents(1);
	mvalues->resizeColumnToContents(2);
	//syntactic elements
	msynmodel->clear();
	msynmodel->insertRows(0,3);
	if(msynmodel->columnCount()<2)
		msynmodel->insertColumns(0,2);
	msynmodel->setHorizontalHeaderLabels(QStringList()<<"Name"<<"Instances");
	//functions
	pidx=msynmodel->index(0,0);
	k=engine.functionNames();
	msynmodel->setData(pidx,"Functions");
	msynmodel->insertRows(0,k.size(),pidx);
	msynmodel->insertColumns(0,1,pidx);
	for(int i=0;i<k.size();i++)
		msynmodel->setData(msynmodel->index(i,0,pidx),k[i]);
	//unary
	pidx=msynmodel->index(1,0);
	k=engine.unaryOperatorNames();
	msynmodel->setData(pidx,"Unary Operators");
	msynmodel->insertRows(0,k.size(),pidx);
	msynmodel->insertColumns(0,2,pidx);
	for(int i=0;i<k.size();i++){
		msynmodel->setData(msynmodel->index(i,0,pidx),k[i]);
		msynmodel->setData(msynmodel->index(i,1,pidx), engine.unaryOperator(k[i]).getTypeIds().size());
	}
	//binary
	pidx=msynmodel->index(2,0);
	msynmodel->setData(pidx,"Binary Operators");
	k=engine.binaryOperatorNames();
	msynmodel->insertRows(0,k.size(),pidx);
	msynmodel->insertColumns(0,2,pidx);
	for(int i=0;i<k.size();i++){
		msynmodel->setData(msynmodel->index(i,0,pidx),k[i]);
		msynmodel->setData(msynmodel->index(i,1,pidx), engine.binaryOperator(k[i]).getTypeIds().size());
	}
	//adjust
	msyntax->expandAll();
	msyntax->resizeColumnToContents(0);
	msyntax->resizeColumnToContents(1);
}


int main(int argc,char**argv)
{
	QApplication app(argc,argv);
	
	//init engine
	ELAM::IntEngine::configureIntEngine(engine);
	ELAM::FloatEngine::configureFloatEngine(engine);
	ELAM::BoolEngine::configureBoolEngine(engine);
	ELAM::BoolEngine::configureLogicEngine(engine);
	ELAM::StringEngine::configureStringEngine(engine);
	
	//init and show window
	ElamDemo d;
	d.show();
	
	return app.exec();
}