#pragma once

#include <QDockWidget>
#include "ui_ProgressDockWidget.h"
#include "scone/core/core.h"
#include "scone/core/String.h"

using scone::String;
class SconeStudio;

class ProgressDockWidget : public QDockWidget
{
public:
	ProgressDockWidget( SconeStudio* s, const QString& config_file );
	virtual ~ProgressDockWidget();
	
	SconeStudio* studio;
	String name;
	QString fileName;
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

	bool updateProgress();
	bool isFinished();

protected:
	virtual void closeEvent( QCloseEvent * ) override;
private:
	void updateText();
};
