#pragma once

#include <QDockWidget>
#include "ui_ProgressDockWidget.h"
#include "scone/core/platform.h"
#include "scone/core/memory_tools.h"
#include "scone/core/PropNode.h"
#include "xo/numerical/polynomial.h"
#include "scone/core/types.h"

using scone::String;
using scone::PropNode;
class SconeStudio;

class ProgressDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	ProgressDockWidget( SconeStudio* s, const QString& config_file, const QStringList& args = QStringList() );
	virtual ~ProgressDockWidget();

	QString& getIdentifier() { return name.isEmpty() ? fileName : name; }

	enum AxisScaleType { Linear, Logarithmic };
	void SetAxisScaleType( AxisScaleType ast, double log_base = 2.0 );

	SconeStudio* studio;
	QString name;
	QString fileName;
	QString errorMsg;
	QString tooltipText;
	QProcess* process;
	Ui::ProgressDockWidget ui;
	int best_gen;
	float best;
	float highest;
	float lowest;
	float cur_best;
	float cur_avg;
	float cur_med;
	float cur_pred;
	xo::linear_function< float > cur_reg;
	int cur_gen;

	int view_first_gen;
	int view_last_gen;
	int min_view_gens = 20;

	int max_generations;
	int window_size;
	QVector< double > bestvec;
	QVector< double > avgvec;
	QVector< double > medvec;
	QVector< double > genvec;
	PropNode status_;

	enum State { StartingState, InitializingState, RunningState, FinishedState, ClosedState, ErrorState };
	State state;

	enum ProgressResult { OkResult, IsClosedResult, FailureResult, ShowErrorResult };
	ProgressResult updateProgress();
	bool readyForDestruction();

public slots:
	void rangeChanged( const QCPRange &newRange, const QCPRange &oldRange );

protected:
	virtual void closeEvent( QCloseEvent * ) override;


private:
	void updateText();
};
