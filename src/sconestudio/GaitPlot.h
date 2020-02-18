#pragma once

#include <QWidget>
#include <QGridLayout>

#include "scone/core/Storage.h"
#include "scone/core/PropNode.h"
#include "scone/core/types.h"
#include "scone/core/GaitCycle.h"
#include "xo/container/flat_map.h"
#include "xo/numerical/bounds.h"

class QCustomPlot;
class QCPPlotTitle;

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

		xo::flat_map<double, xo::bounds<double>> norm_data_;
		
	private:
		QCustomPlot* plot_;
		QCPPlotTitle* plot_title_;
	};
}
