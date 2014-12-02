#include "stdafx.h"
#include "SconeStudio.h"
#include <QtGui/QApplication>
#include "../sim/simbody/Driver_Simbody.h"
#include "../sim/sim.h"
#include "../sim/World.h"

using namespace scone;

int main(int argc, char *argv[])
{
	sim::Driver_Simbody driver;
	//sim::WorldUP world = driver.CreateWorld();
	//sim::ModelUP model = world->CreateModel( "../../models/test.osim" );

	//cs::FeedForwardController ff_control;
	//cs::JumpingObjective objective( driver, "test.osim", ff_control );
	//opt::OptimizerCma optimizer( objective );

	//optimizer.SetWorld( world );
	//optimizer




	//QApplication a(argc, argv);
	//SconeStudio w;
	//w.show();
	//return a.exec();
}
