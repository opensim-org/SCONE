#include "help_tools.h"
#include "xo/container/flat_map.h"
#include "QFile"
#include "xo/filesystem/filesystem.h"
#include "scone/core/system_tools.h"
#include "QDesktopServices"

namespace scone
{
	QUrl GetHelpUrl( const QString& keyword )
	{
		static xo::flat_map<QString, QString> data;
		if ( data.empty() )
		{
			auto vec = xo::split_str( xo::load_string( GetFolder( SCONE_RESOURCE_FOLDER ) / "help/keywords.txt" ), "\n" );
			for ( const auto& k : vec )
			{
				QString doku = QString( k.c_str() ).remove( ".txt" );
				QString keyword = QString( doku ).remove( '_' );
				data[ keyword ] = doku;
			}
		}

		if ( !keyword.isEmpty() )
		{
			auto k = keyword.toLower();
			if ( auto it = data.find( k ); it != data.end() )
				return QUrl( "https://scone.software/doku.php?id=ref:" + it->second );
			else return QUrl( "https://scone.software/doku.php?do=search&q=" + keyword );
		}
		else return QUrl( "https://scone.software/doku.php?id=start" );
	}
}
