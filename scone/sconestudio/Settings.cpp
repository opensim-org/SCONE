#include "Settings.h"

#include <QDialog>
#include "ui_SconeSettings.h"
#include "scone/core/PropNode.h"
#include "scone/core/system_tools.h"
#include "qt_tools.h"

namespace scone
{
	int Settings::showDialog( QWidget* parent )
	{
		QDialog* dlg = new QDialog( parent );
		Ui::Settings settings;
		settings.setupUi( dlg );

		// init settings
		const PropNode& pn = GetSconeSettings();
		settings.scenariosFolder->setText( make_qt( GetFolder( SCONE_SCENARIO_FOLDER ) ) );
		settings.modelsFolder->setText( make_qt( GetFolder( SCONE_MODEL_FOLDER ) ) );
		settings.resultsFolder->setText( make_qt( GetFolder( SCONE_OUTPUT_FOLDER ) ) );
		settings.geometryFolder->setText( make_qt( GetFolder( SCONE_GEOMETRY_FOLDER ) ) );

		int ret = dlg->exec();
		if ( ret == QDialog::Accepted )
		{
			// update settings
			// ...
		}

		return ret;
	}
}
