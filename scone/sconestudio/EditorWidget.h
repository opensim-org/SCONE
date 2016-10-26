#pragma once

#include "scone/core/core.h"
#include "scone/core/String.h"
#include "BasicXMLSyntaxHighlighter/BasicXMLSyntaxHighlighter.h"
#include <QWidget>
#include <QFileInfo>
#include "QCodeEditor.h"

using scone::String;
class SconeStudio;

class EditorWidget : public QWidget
{
	Q_OBJECT

public:
	EditorWidget( SconeStudio* s, const QString& file );
	virtual ~EditorWidget();
	bool hasTextChanged() { return textChangedFlag; }

public slots:
	void save();
	void saveAs();
	QString getTitle() { return QFileInfo( fileName ).fileName() + ( hasTextChanged() ? "*" : "" ); }
	void textEditChanged();

signals:
	void textChanged();

public:
	SconeStudio* studio;
	QString fileName;
	QString fileData;
	bool textChangedFlag = false;
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter;

protected:
	//virtual void closeEvent( QCloseEvent * ) override;

private:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QCodeEditor *textEdit;
};
