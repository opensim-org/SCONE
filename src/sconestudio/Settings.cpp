#include "Settings.h"

#include <QDialog>
#include "ui_SconeSettings.h"
#include "scone/core/PropNode.h"
#include "scone/core/system_tools.h"
#include "qt_tools.h"
#include "xo/filesystem/path.h"

namespace scone
{
	int Settings::showDialog( QWidget* parent )
	{
		QDialog* dlg = new QDialog( parent );
		Ui::Settings settings;
		settings.setupUi( dlg );

		// init settings
		PropNode pn = GetSconeSettings();
		settings.scenariosFolder->setText( make_qt( xo::path( GetFolder( SCONE_SCENARIO_FOLDER ) ).make_preferred() ) );
		//settings.modelsFolder->setText( make_qt( xo::path( GetFolder( SCONE_MODEL_FOLDER ) ).make_preferred() ) );
		settings.resultsFolder->setText( make_qt( xo::path( GetFolder( SCONE_RESULTS_FOLDER ) ).make_preferred() ) );
		settings.geometryFolder->setText( make_qt( xo::path( GetFolder( SCONE_GEOMETRY_FOLDER ) ).make_preferred() ) );

		int ret = dlg->exec();
		if ( ret == QDialog::Accepted )
		{
			// update settings
			auto& pf = pn.get_or_add_child( "folders" );
			pf.set( "scenarios", settings.scenariosFolder->text().toStdString() );
			//pf.set( "models", settings.modelsFolder->text().toStdString() );
			pf.set( "results", settings.resultsFolder->text().toStdString() );
			pf.set( "geometry", settings.geometryFolder->text().toStdString() );

			SaveSconeSettings( pn );
		}

		return ret;
	}
}
