#pragma once

#include <QWidget>

namespace scone
{
	class Settings
	{
	public:
		Settings() {}
		virtual ~Settings() {}

		int showDialog( QWidget* parent );
		
	protected:
	private:
	};
}
