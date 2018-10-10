/*
** SettingsEditor.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SettingsEditor.h"

#include <QDialog>
#include "qt_tools.h"
#include "QPropNodeItemModel.h"

#include "ui_SconeSettings.h"
#include "scone/core/PropNode.h"
#include "scone/core/system_tools.h"
#include "scone/core/Settings.h"
#include "scone/core/Log.h"
#include "xo/filesystem/path.h"
#include "xo/system/type_class.h"
#include "xo/container/flat_map.h"
#include "StudioSettings.h"

namespace scone
{
	int SettingsEditor::showDialog( QWidget* parent )
	{
		QDialog dialog_window( parent );
		Ui::Settings ui;
		ui.setupUi( &dialog_window );

		// init settings
		auto& sconecfg = GetSconeSettings();
		auto& studiocfg = GetStudioSettings();

		// folders
		ui.scenariosFolder->setText( make_qt( xo::path( GetFolder( SCONE_SCENARIO_FOLDER ) ).make_preferred() ) );
		ui.resultsFolder->setText( make_qt( xo::path( GetFolder( SCONE_RESULTS_FOLDER ) ).make_preferred() ) );
		ui.geometryFolder->setText( make_qt( xo::path( GetFolder( SCONE_GEOMETRY_FOLDER ) ).make_preferred() ) );

		// data checkboxes
		xo::flat_map< string, QListWidgetItem* > data_checkboxes;
		for ( auto& item : sconecfg.schema().get_child( "data" ) )
		{
			if ( item.second.get<string>( "type" ) == "bool" )
			{
				auto* checkbox = new QListWidgetItem( item.second.get< string >( "description" ).c_str() );
				checkbox->setCheckState( sconecfg.get< bool >( "data." + item.first ) ? Qt::Checked : Qt::Unchecked );
				ui.dataList->addItem( checkbox );
				data_checkboxes[ item.first ] = checkbox;
			}
		}

		// advanced settings
		auto scone_pn = sconecfg.data();
		auto* advancedModel = new QPropNodeItemModel( scone_pn );
		ui.advancedTree->setModel( advancedModel );
		ui.advancedTree->expandAll();

		// studio settings
		auto studio_pn = studiocfg.data();
		auto* studioModel = new QPropNodeItemModel( studio_pn );
		ui.studioTree->setModel( studioModel );
		ui.studioTree->expandAll();

		int ret = dialog_window.exec();
		if ( ret == QDialog::Accepted )
		{
			sconecfg.set( scone_pn );
			studiocfg.set( studio_pn );

			// update settings
			sconecfg.set( "folders.scenarios", ui.scenariosFolder->text().toStdString() );
			sconecfg.set( "folders.results", ui.resultsFolder->text().toStdString() );
			sconecfg.set( "folders.geometry", ui.geometryFolder->text().toStdString() );

			// copy checkboxes
			for ( auto& item : data_checkboxes )
				sconecfg.set< bool >( "data." + item.first, item.second->checkState() == Qt::Checked );

			GetSconeSettings().save();
			GetStudioSettings().save();
		}

		return ret;
	}
}
