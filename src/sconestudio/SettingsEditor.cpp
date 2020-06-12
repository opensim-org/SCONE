/*
** SettingsEditor.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SettingsEditor.h"

#include <QDialog>
#include "qt_convert.h"
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
#include "QSettingsItemModel.h"
#include "scone/sconelib_config.h"

namespace scone
{
	int SettingsEditor::showDialog( QWidget* parent )
	{
		QDialog dialog_window( parent );
		Ui::Settings ui;
		ui.setupUi( &dialog_window );

		// init SCONE settings
		auto& scone_settings = GetSconeSettings();

		// folders
		ui.scenariosFolder->setText( to_qt( xo::path( GetFolder( SCONE_SCENARIO_FOLDER ) ).make_preferred() ) );
		ui.resultsFolder->setText( to_qt( xo::path( GetFolder( SCONE_RESULTS_FOLDER ) ).make_preferred() ) );
		ui.geometryFolder->setText( to_qt( xo::path( GetFolder( SCONE_GEOMETRY_FOLDER ) ).make_preferred() ) );
		ui.gaitAnalysisFolder->setText( to_qt( GetStudioSetting<xo::path>( "gait_analysis.template" ).make_preferred() ) );

		// data checkboxes
		xo::flat_map< string, QListWidgetItem* > data_checkboxes;
		for ( auto& item : scone_settings.schema().get_child( "data" ) )
		{
			if ( item.second.get<string>( "type" ) == "bool" )
			{
				auto* checkbox = new QListWidgetItem( item.second.get<string>( "label" ).c_str() );
				checkbox->setCheckState( scone_settings.get< bool >( "data." + item.first ) ? Qt::Checked : Qt::Unchecked );
				ui.dataList->addItem( checkbox );
				data_checkboxes[ item.first ] = checkbox;
			}
		}

		// advanced settings
		auto* advancedModel = new QSettingsItemModel( scone_settings );
		ui.advancedTree->setModel( advancedModel );
		ui.advancedTree->expandAll();

		// init STUDIO settings
		auto& studio_settings = GetStudioSettings();
		auto* studioModel = new QSettingsItemModel( studio_settings );
		ui.studioTree->setModel( studioModel );
		ui.studioTree->expandAll();

#ifdef SCONE_HYFYDY
		bool hfd_enabled = scone_settings.get<bool>( "hyfydy.enabled" );
		auto hfd_license = scone_settings.get<String>( "hyfydy.license" );
		ui.hfdEnabled->setCheckState( hfd_enabled ? Qt::Checked : Qt::Unchecked );
		ui.hfdLicenseKey->setPlainText( to_qt( hfd_license ) );
#else
		ui.tabWidget->removeTab( ui.tabWidget->indexOf( ui.hfdTab ) );
#endif

		for ( int i = 0; i < 3; i++ )
		{
			ui.advancedTree->resizeColumnToContents( i );
			ui.studioTree->resizeColumnToContents( i );
		}

		int ret = dialog_window.exec();
		if ( ret == QDialog::Accepted )
		{
			// update settings
			scone_settings.set( "folders.scenarios", ui.scenariosFolder->text().toStdString() );
			scone_settings.set( "folders.results", ui.resultsFolder->text().toStdString() );
			scone_settings.set( "folders.geometry", ui.geometryFolder->text().toStdString() );
			studio_settings.set( "gait_analysis.template", ui.gaitAnalysisFolder->text().toStdString() );

			// copy checkboxes
			for ( auto& item : data_checkboxes )
				scone_settings.set< bool >( "data." + item.first, item.second->checkState() == Qt::Checked );

#ifdef SCONE_HYFYDY
			auto hfd_new_license = ui.hfdLicenseKey->toPlainText().toStdString();
			auto hfd_new_enabled = ui.hfdEnabled->isChecked();
			if ( hfd_new_license != hfd_license || hfd_enabled != hfd_new_enabled )
			{
				scone_settings.set( "hyfydy.license", hfd_new_license );
				scone_settings.set( "hyfydy.enabled", hfd_new_enabled );
				if ( hfd_new_enabled )
					RegisterSconeHfd( GetSconeSetting<String>( "hyfydy.license" ).c_str() );
			}
#endif
			scone_settings.save();
			studio_settings.save();
		}
		else
		{
			// cancel was pressed, reload old settings
			scone_settings.load();
			studio_settings.load();
		}

		return ret;
	}
}
