#include "model_conversion.h"

#include "ui_ModelTool.h"
#include "scone/core/Log.h"
#include "scone/core/system_tools.h"
#include "qt_convert.h"
#include "xo/filesystem/path.h"

#include <QProcess>
#include <QMessageBox>
#include <QPushButton>

namespace scone
{
	void ShowModelConversionDialog( QWidget* parent )
	{
		QDialog dlg( parent );
		Ui::ModelTool ui;
		ui.setupUi( &dlg );
		ui.inputFile->init( QFileEdit::OpenFile, "OpenSim Models (*.osim)", "", to_qt( GetFolder( SCONE_SCENARIO_FOLDER ) ) );
		auto updateInputFile = [&]() {
			auto inputFile = xo::path( ui.inputFile->text().toStdString() );
			ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( !inputFile.empty() );
			if ( !inputFile.empty() )
				ui.outputFile->setText( to_qt( inputFile.replace_extension( "hfd" ) ) );
		};
		updateInputFile();
		QObject::connect( ui.inputFile, &QFileEdit::textChanged, updateInputFile );

		if ( QDialog::Accepted == dlg.exec() )
		{
			const xo::path inputFile = xo::path( ui.inputFile->text().toStdString() );
			const xo::path outputFile = xo::path( ui.outputFile->text().toStdString() );
			log::info( "Converting model ", inputFile );

			QString program = to_qt( GetApplicationFolder() / "hfdmodeltool" );
			QStringList args;
			args << to_qt( inputFile ) << "-o" << to_qt( outputFile );
			args << "--remote";
			if ( ui.fixCheckbox->isChecked() )
				args << "-f";

			auto proc = new QProcess( parent );
			proc->start( program, args );
			if ( !proc->waitForFinished() )
			{
				QString msg = "Timeout waiting for " + program;
				log::error( msg.toStdString() );
				QMessageBox::critical( parent, "Error", msg );
				return;
			}

			// check return code and output
			QString output = proc->readAllStandardOutput();
			if ( proc->exitCode() == 0 )
			{
				QString title = "Successfully converted " + ui.inputFile->text();
				log::info( title.toStdString() );
				log::info( output.toStdString() );
				QMessageBox::information( parent, title, output );
			}
			else
			{
				QString title = "Error converting " + ui.inputFile->text() + "\n\n" + output;
				log::error( title.toStdString() );
				log::error( output.toStdString() );
				QMessageBox::critical( parent, title, output );
			}
		}
	}
}
