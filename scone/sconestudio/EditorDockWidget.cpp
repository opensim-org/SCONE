#include "EditorDockWidget.h"
#include "SconeStudio.h"
#include "qt_tools.h"
#include "flut/system_tools.hpp"
#include "flut/system/assert.hpp"
#include "scone/core/Log.h"
#include "flut/string_tools.hpp"
#include "scone/core/system_tools.h"

using namespace scone;

EditorDockWidget::EditorDockWidget( SconeStudio* s, const QString& file ) :
studio( s ),
fileName( file )
{
	ui.setupUi( this );
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter;
	ui.textEdit->setTabStopWidth( 16 );
	ui.textEdit->setWordWrapMode( QTextOption::NoWrap );
	xmlSyntaxHighlighter = new BasicXMLSyntaxHighlighter( ui.textEdit );
	setWindowTitle( fileName );

	QFile f( fileName );
	if ( f.open( QFile::ReadOnly | QFile::Text ) )
	{
		QTextStream str( &f );
		ui.textEdit->setText( str.readAll() );
	}
}

EditorDockWidget::~EditorDockWidget()
{
}

void EditorDockWidget::save()
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
		stream << ui.textEdit->toPlainText();
		stream.flush();
		file.close();
	}
}

void EditorDockWidget::saveAs()
{
	QString fn = QFileDialog::getSaveFileName( this, "Save Scenario", QString( scone::GetFolder( SCONE_SCENARIO_FOLDER ).c_str() ), "SCONE Scenarios (*.xml)" );
	if ( !fn.isEmpty() )
	{
		fileName = fn;
		save();
	}
}

void EditorDockWidget::closeEvent( QCloseEvent *e )
{
}
