/*
** SettingsEditor.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

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
