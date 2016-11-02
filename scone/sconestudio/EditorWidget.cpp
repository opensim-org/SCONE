#include "EditorWidget.h"
#include "SconeStudio.h"
#include "qt_tools.h"
#include "flut/system_tools.hpp"
#include "flut/system/assert.hpp"
#include "scone/core/Log.h"
#include "flut/string_tools.hpp"
#include "scone/core/system_tools.h"

using namespace scone;

EditorWidget::EditorWidget( SconeStudio* s, const QString& file ) :
studio( s ),
fileName( file )
{
	verticalLayout = new QVBoxLayout( this );
	verticalLayout->setObjectName( QStringLiteral( "verticalLayout" ) );
	setLayout( verticalLayout );

	textEdit = new QCodeEditor( this );
	textEdit->setObjectName( QStringLiteral( "textEdit" ) );

	QFont font;
	font.setFamily( QStringLiteral( "Consolas" ) );
	font.setPointSize( 9 );
	textEdit->setFont( font );
	textEdit->setLineWrapMode( QPlainTextEdit::NoWrap );
	textEdit->setTabStopWidth( 16 );
	textEdit->setWordWrapMode( QTextOption::NoWrap );

	verticalLayout->addWidget( textEdit );
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter = new BasicXMLSyntaxHighlighter( textEdit->document() );

	QFile f( fileName );
	if ( f.open( QFile::ReadOnly | QFile::Text ) )
	{
		QTextStream str( &f );
		fileData = str.readAll();
		textEdit->setPlainText( fileData );
		//textEdit->updateLineNumberAreaWidth( 0 );
	}

	connect( textEdit, SIGNAL( textChanged() ), this, SLOT( textEditChanged() ) );
}

EditorWidget::~EditorWidget()
{
}

void EditorWidget::save()
{
	QFile file( fileName );
	if ( !file.open( QIODevice::WriteOnly ) )
	{
		QMessageBox::critical( this, "Error writing file", "Could not open file " + fileName );
		return;
	}
	else
	{
		QTextStream stream( &file );
		stream << textEdit->toPlainText();
		stream.flush();
		file.close();
		textChangedFlag = false;
	}
}

void EditorWidget::saveAs()
{
	QString fn = QFileDialog::getSaveFileName( this, "Save Scenario", make_qt( scone::GetFolder( SCONE_SCENARIO_FOLDER ) ), "SCONE Scenarios (*.xml)" );
	if ( !fn.isEmpty() )
	{
		fileName = fn;
		save();
	}
}

void EditorWidget::textEditChanged()
{
	if ( !textChangedFlag )
	{
		textChangedFlag = true;
		emit textChanged();
	}
}

//void EditorDockWidget::closeEvent( QCloseEvent *e )
//{
//}
