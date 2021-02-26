/*
** ProgressDockWidget.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <QDockWidget>
#include "ui_ProgressDockWidget.h"
#include "scone/core/platform.h"
#include "scone/core/memory_tools.h"
#include "scone/core/PropNode.h"
#include "xo/numerical/polynomial.h"
#include "scone/core/types.h"
#include "qt_convert.h"
#include "OptimizerTask.h"

using scone::String;
using scone::PropNode;
class SconeStudio;

class ProgressDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	enum State { StartingState, RunningState, FinishedState, ClosedState, ErrorState };
	enum ProgressResult { OkResult, IsClosedResult, FailureResult, ShowErrorResult };

	ProgressDockWidget( SconeStudio* s, std::unique_ptr<scone::OptimizerTask> task );
	virtual ~ProgressDockWidget();

	QString getIdentifier() { return optimizations.empty() ? "" : to_qt( optimizations.front().name ); }
	ProgressResult updateProgress();
	bool readyForDestruction() const;
	bool canCloseWithoutWarning() const;
	void disableCloseWarning() { showCloseWarning = false; }

	enum AxisScaleType { Linear, Logarithmic };
	void SetAxisScaleType( AxisScaleType ast, double log_base = 2.0 );

	SconeStudio* studio;
	std::unique_ptr<scone::OptimizerTask> task_;
	Ui::ProgressDockWidget ui;
	State state;
	String message;
	PropNode tooltipProps;
	QString tooltipText;

	struct Optimization
	{
		int idx;
		String name;
		String message;
		State state;
		int max_generations;
		int window_size;
		bool is_minimizing;

		int best_gen = 0;
		float best = 0.0f;
		int cur_gen = 0;
		float cur_pred = 0.0f;
		xo::linear_function< float > cur_reg;

		double duration;

		QVector< double > bestvec;
		QVector< double > medvec;
		QVector< double > genvec;

		void Update( const PropNode& pn );
		bool has_update_flag = false;
	};

	bool showCloseWarning;
	bool closeWhenFinished;

	int min_view_gens;
	int view_first_gen;
	int view_last_gen;

	int best_idx;
	bool minimize;
	String scenario;

	std::vector< Optimization > optimizations;

public slots:
	void rangeChanged( const QCPRange &newRange, const QCPRange &oldRange );
	void fixRangeY();

protected:
	virtual void closeEvent( QCloseEvent * ) override;

private:
	void updateText();
};
