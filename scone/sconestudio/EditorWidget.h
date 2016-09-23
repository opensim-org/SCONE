#pragma once

#include "ui_EditorDockWidget.h"
#include "scone/core/core.h"
#include "scone/core/String.h"
#include "BasicXMLSyntaxHighlighter/BasicXMLSyntaxHighlighter.h"
#include <QWidget>
#include <QFileInfo>

using scone::String;
class SconeStudio;

class EditorWidget : public QWidget
{
public:
	EditorWidget( SconeStudio* s, const QString& file );
	virtual ~EditorWidget();

public slots:
	void save();
	void saveAs();
	QString getTitle() { return QFileInfo( fileName ).fileName(); }

public:
	SconeStudio* studio;
	QString fileName;
	bool fileChanged = false;
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter;

protected:
	//virtual void closeEvent( QCloseEvent * ) override;

private:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
};
