#pragma once

#include "scone/sim/Model.h"
#include "osg/Group"
#include "osg/Geode"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( OsgModel );

	class OsgModel
	{
	public:
		OsgModel( const sim::Model& m );
		virtual ~OsgModel() {}
		void Update();

		osg::ref_ptr< osg::Group > GetOsgRoot() { return root; }
		
	private:
		osg::ref_ptr< osg::Group > root;
		osg::ref_ptr< osg::Geode > body;
		const sim::Model& model;
	};
}
