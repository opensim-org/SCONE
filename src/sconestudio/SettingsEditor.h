#pragma once

#include <QWidget>

namespace scone
{
	class SettingsEditor
	{
	public:
		SettingsEditor() {}
		virtual ~SettingsEditor() {}

		int showDialog( QWidget* parent );
	};
}
