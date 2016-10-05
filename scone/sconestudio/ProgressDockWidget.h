#pragma once

#include <QDockWidget>
#include "ui_ProgressDockWidget.h"
#include "scone/core/core.h"
#include "scone/core/String.h"
#include "scone/core/PropNode.h"

using scone::String;
using scone::PropNode;
class SconeStudio;

class ProgressDockWidget : public QDockWidget
{
public:
	ProgressDockWidget( SconeStudio* s, const QString& config_file, const QStringList& args = QStringList() );
	virtual ~ProgressDockWidget();

	QString& getIdentifier() { return name.isEmpty() ? fileName : name; } 
	
	SconeStudio* studio;
	QString name;
	QString fileName;
	QString errorMsg;
	QProcess* process;
	Ui::ProgressDockWidget ui;
	int best_gen;
	float best;
	float highest;
	float lowest;
	float cur_best;
	float cur_avg;
	int generation;
	int max_generations;
	QVector< double > bestvec;
	QVector< double > avgvec;
	QVector< double > genvec;

	enum State { StartingState, InitializingState, RunningState, FinishedState, ClosedState, ErrorState };
	State state;

	void updateProgress();
	bool isClosed();

protected:
	virtual void closeEvent( QCloseEvent * ) override;
private:
	void updateText();
};
