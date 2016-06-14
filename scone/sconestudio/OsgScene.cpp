#include "OsgScene.h"

namespace scone
{
	OsgScene::OsgScene()
	{

	}

	OsgScene::~OsgScene()
	{

	}

	scone::OsgModel& OsgScene::CreateModel( sim::Model& m )
	{
		models.push_back( std::make_unique< OsgModel >( m ) );
		return *models.back();
	}

	void OsgScene::UpdateModels()
	{
		for ( auto& m : models )
			m->Update();
	}
}
