#pragma once

#include "scone/model/Model.h"
#include "vis/trail.h"
#include "vis/material.h"
#include "vis/plane.h"
#include "vis/axes.h"
#include "vis/arrow.h"
#include "xo/utility/color_gradient.h"

namespace scone
{
	class ModelVis
	{
	public:
		ModelVis( const Model& model, vis::scene& s );
		~ModelVis();

		void Update( const Model& model );

		enum VisOpt { ShowForces, ShowMuscles, ShowTendons, ShowGeometry, ShowJoints, ShowAxes, ShowContactGeom, ShowGroundPlane, EnableShadows };
		using ViewSettings = xo::flag_set< VisOpt >;

		void ApplyViewSettings( const ViewSettings& f );

	private:
		struct MuscleVis
		{
			vis::trail ten1;
			vis::trail ten2;
			vis::trail ce;
			vis::material mat;
			float ce_pos = 0.5f;
		};

		void UpdateForceVis( index_t force_idx, Vec3 cop, Vec3 force );
		void UpdateMuscleVis( const class Muscle& mus, MuscleVis& vis );

		// view settings
		ViewSettings view_flags;
		vis::plane ground_;
		vis::node root_node_;
		float specular_;
		float shininess_;
		float ambient_;
		vis::material bone_mat;
		vis::material joint_mat;
		vis::material muscle_mat;
		vis::material tendon_mat;
		vis::material arrow_mat;
		vis::material contact_mat;
		xo::color_gradient muscle_gradient;
		std::vector< vis::mesh > body_meshes;
		std::vector< vis::mesh > joints;
		std::vector< MuscleVis > muscles;
		std::vector< vis::arrow > forces;
		std::vector< vis::axes > body_axes;
		std::vector< vis::node > bodies;
		std::vector< vis::mesh > contact_geoms;
	};
}
