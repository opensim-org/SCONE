#pragma once

#include <QDockWidget>
#include "ui_EditorDockWidget.h"
#include "scone/core/core.h"
#include "scone/core/String.h"
#include "BasicXMLSyntaxHighlighter/BasicXMLSyntaxHighlighter.h"

using scone::String;
class SconeStudio;

class EditorDockWidget : public QDockWidget
{
public:
	EditorDockWidget( SconeStudio* s, const QString& file );
	virtual ~EditorDockWidget();

public slots:
	void save();
	void saveAs();

public:
	SconeStudio* studio;
	QString fileName;
	Ui::EditorDockWidget ui;
	bool fileChanged = false;
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter;

protected:
	virtual void closeEvent( QCloseEvent * ) override;
};
