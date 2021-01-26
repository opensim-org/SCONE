#include "model_tools.h"

#include "ui_ModelTool.h"
#include "scone/core/Log.h"
#include "scone/core/system_tools.h"
#include "qt_convert.h"
#include "xo/filesystem/path.h"
#include "QProcess"

namespace scone
{
	void ShowModelConversionDialog( QWidget* parent )
	{
		QDialog dlg( parent );
		Ui::ModelTool ui;
		ui.setupUi( &dlg );
		ui.inputFile->init( QFileEdit::OpenFile, "OpenSim Models (*.osim)", "", to_qt( GetFolder( SCONE_SCENARIO_FOLDER ) ) );
		auto updateButtonBox = [&]() {
			ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( !ui.inputFile->text().isEmpty() );
		};
		updateButtonBox();
		QObject::connect( ui.inputFile, &QFileEdit::textChanged, updateButtonBox );


		if ( QDialog::Accepted == dlg.exec() )
		{
			xo::path inputFile = xo::path( ui.inputFile->text().toStdString() );
			log::info( "Converting model ", inputFile );

			QString program = to_qt( GetApplicationFolder() / "hfdmodeltool" );
			QStringList args;
			args << "-c" << to_qt( inputFile );

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
