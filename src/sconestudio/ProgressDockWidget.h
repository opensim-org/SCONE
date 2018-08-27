#pragma once

#include <QDockWidget>
#include "ui_ProgressDockWidget.h"
#include "scone/core/platform.h"
#include "scone/core/memory_tools.h"
#include "scone/core/PropNode.h"
#include "xo/numerical/polynomial.h"
#include "scone/core/types.h"
#include "qt_tools.h"

using scone::String;
using scone::PropNode;
class SconeStudio;

class ProgressDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	enum State { StartingState, InitializingState, RunningState, FinishedState, ClosedState, ErrorState };
	enum ProgressResult { OkResult, IsClosedResult, FailureResult, ShowErrorResult };

	ProgressDockWidget( SconeStudio* s, const QString& config_file, const QStringList& args = QStringList() );
	virtual ~ProgressDockWidget();

	QString getIdentifier() { return optimizations.empty() ? "" : make_qt( optimizations.front().name ); }

	enum AxisScaleType { Linear, Logarithmic };
	void SetAxisScaleType( AxisScaleType ast, double log_base = 2.0 );

	SconeStudio* studio;
	QProcess* process;
	Ui::ProgressDockWidget ui;
	QString fileName;
	State state;
	QString errorMsg;
	PropNode tooltipProps;
	QString tooltipText;

	struct Optimization
	{
		int idx;
		String name;
		int max_generations;
		int window_size;

		int best_gen = 0;
		float best = 0.0f;
		int cur_gen = 0;
		float cur_pred = 0.0f;
		xo::linear_function< float > cur_reg;

		QVector< double > bestvec;
		QVector< double > avgvec;
		QVector< double > genvec;

		void Update( const PropNode& pn, ProgressDockWidget& wdg );
		bool has_update_flag = false;
	};

	int min_view_gens;
	int view_first_gen;
	int view_last_gen;

	std::vector< Optimization > optimizations;

	ProgressResult updateProgress();
	bool readyForDestruction();

public slots:
	void rangeChanged( const QCPRange &newRange, const QCPRange &oldRange );
	void fixRangeY();

protected:
	virtual void closeEvent( QCloseEvent * ) override;


private:
	void updateText();
};
