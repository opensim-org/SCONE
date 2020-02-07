#pragma once

#include <QWidget>
#include <QGridLayout>

#include "scone/core/Storage.h"
#include "scone/core/PropNode.h"
#include "scone/core/types.h"
#include "scone/core/GaitCycle.h"

class QCustomPlot;

namespace scone
{
	class GaitPlot : public QWidget
	{
	public:
		GaitPlot( const PropNode& pn, QWidget* parent = nullptr );
		virtual ~GaitPlot() {}

		void update( const Storage<>& sto, const std::vector<GaitCycle>& cycles );

		String title_;
		String left_channel_;
		String right_channel_;
		int row_;
		int column_;
		String x_label_;
		String y_label_;
		double y_min_;
		double y_max_;

		double channel_offset_;
		double channel_multiply_;
		double norm_offset_;
		
	private:
		QCustomPlot* plot_;
	};
}
