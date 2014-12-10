#include "stdafx.h"
#include "SconeStudio.h"
#include <QtGui/QApplication>
#include "../opt/Optimization.h"

using namespace scone;

int main(int argc, char *argv[])
{
	opt::Optimization o;
	o.Run( "../../scripts/optimizations/test.opt" );

	//QApplication a(argc, argv);
	//SconeStudio w;
	//w.show();
	//return a.exec();
}
