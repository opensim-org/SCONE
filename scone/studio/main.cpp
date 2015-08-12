#include "stdafx.h"
#include "SconeStudio.h"
#include <QtWidgets/QApplication>
#include <QMessageBox.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SconeStudio w;

	try
	{
		w.init();
		w.show();
		return a.exec();
	}
	catch ( std::exception& e )
	{
		QMessageBox::critical( 0, "Exception", e.what() );
	}
	catch ( ... )
	{
		QMessageBox::critical( 0, "Exception", "Unknown Exception" );
	}
}
