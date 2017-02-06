#include "QCodeEditor.h"
#include "SconeStudio.h"
#include "qt_tools.h"
#include "flut/system_tools.hpp"
#include "flut/system/assert.hpp"
#include "scone/core/Log.h"
#include "flut/string_tools.hpp"
#include "scone/core/system_tools.h"

using namespace scone;

QCodeEditor::QCodeEditor( QWidget* parent ) :
QWidget( parent )
{
	QVBoxLayout* verticalLayout = new QVBoxLayout( this );
	setLayout( verticalLayout );
	textEdit = new QCodeTextEdit( this );

	QFont font;
	font.setFamily( QStringLiteral( "Consolas" ) );
	font.setPointSize( 9 );
	textEdit->setFont( font );
	textEdit->setLineWrapMode( QPlainTextEdit::NoWrap );
	textEdit->setTabStopWidth( 16 );
	textEdit->setWordWrapMode( QTextOption::NoWrap );
	verticalLayout->addWidget( textEdit );

	connect( textEdit, SIGNAL( textChanged() ), this, SLOT( textEditChanged() ) );
}

QCodeEditor::~QCodeEditor()
{}

void QCodeEditor::open( const QString& filename )
{
	QFile f( filename );
	if ( f.open( QFile::ReadOnly | QFile::Text ) )
	{
		QTextStream str( &f );
		data = str.readAll();
		textEdit->setPlainText( data );
		fileName = filename;
	}
	else return;

	// TODO: create appropriate highlighter
	BasicXMLSyntaxHighlighter* xmlSyntaxHighlighter = new BasicXMLSyntaxHighlighter( textEdit->document() );
}

void QCodeEditor::openDialog( const QString& folder, const QString& fileTypes )
{
	auto fn = QFileDialog::getOpenFileName( this, "Open File", folder, fileTypes );
	if ( !fn.isEmpty() )
		open( fn );
}

void QCodeEditor::save()
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

void QCodeEditor::saveAsDialog( const QString& folder, const QString& fileTypes )
{
	QString fn = QFileDialog::getSaveFileName( this, "Save File As", folder, fileTypes );
	if ( !fn.isEmpty() )
	{
		fileName = fn;
		save();
	}
}

QString QCodeEditor::getTitle()
{
	return QFileInfo( fileName ).fileName() + ( hasTextChanged() ? "*" : "" );
}

void QCodeEditor::textEditChanged()
{
	if ( !textChangedFlag )
	{
		textChangedFlag = true;
		emit textChanged();
	}
}
