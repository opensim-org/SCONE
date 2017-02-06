#pragma once

#include "scone/core/core.h"
#include "scone/core/String.h"
#include "BasicXMLSyntaxHighlighter/BasicXMLSyntaxHighlighter.h"
#include <QWidget>
#include <QFileInfo>
#include "QCodeTextEdit.h"

using scone::String;
class SconeStudio;

class QCodeEditor : public QWidget
{
	Q_OBJECT

public:
	QCodeEditor( QWidget* parent = 0 );
	virtual ~QCodeEditor();
	bool hasTextChanged() { return textChangedFlag; }

public slots:
	void open( const QString& filename );
	void openDialog( const QString& folder, const QString& fileTypes );
	void save();
	void saveAsDialog( const QString& folder, const QString& fileTypes );

	QString getTitle();
	void textEditChanged();

signals:
	void textChanged();

public:
	QString defaultFolder;
	QString fileTypes;
	QString fileName;
	QString data;
	bool textChangedFlag = false;
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter;

private:
    QCodeTextEdit *textEdit;
};
