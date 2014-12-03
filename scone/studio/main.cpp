#include "stdafx.h"
#include "SconeStudio.h"
#include <QtGui/QApplication>

#include "../cs/Optimization.h"

using namespace scone;

int main(int argc, char *argv[])
{
	cs::Optimization opt;
	opt.Run( "../../scripts/optimizations/test.opt" );

	//QApplication a(argc, argv);
	//SconeStudio w;
	//w.show();
	//return a.exec();
}
