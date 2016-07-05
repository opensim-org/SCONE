#pragma once

#include "scone/sim/Model.h"
#include "osg/Group"
#include "osg/Geode"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( ModelVis );

	class ModelVis
	{
	public:
		ModelVis( const sim::Model& m );
		virtual ~ModelVis() {}
		void Update();

		osg::ref_ptr< osg::Group > GetOsgRoot() { return root; }
		
	private:
		osg::ref_ptr< osg::Group > root;
		osg::ref_ptr< osg::Geode > body;
		const sim::Model& model;
	};
}
