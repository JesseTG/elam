#ifndef ELAM_DEMO_H
#define ELAM_DEMO_H

#include <QDialog>

class QStandardItemModel;
class QTreeView;
class QTabWidget;
class QLabel;
class QLineEdit;
///Demo Window
class ElamDemo:public QDialog
{
	Q_OBJECT
	public:
		///instantiate the demo window
		ElamDemo(QWidget* parent = 0, Qt::WindowFlags f = 0);
	public slots:
		///executes the user's expression
		void execute();
		///update tables
		void updateViews();
	private:
		QTabWidget*mtabs;
		QLineEdit*mexpression;
		QLabel*mresult;
		QTreeView*mvalues,*msyntax;
		QStandardItemModel*mvalmodel,*msynmodel;
};

#endif
