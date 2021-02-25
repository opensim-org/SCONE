#include "sconeopensim4.h"

#include "scone/core/Factories.h"
#include "ModelOpenSim4.h"
#include "xo/filesystem/path.h"

namespace scone
{
	void RegisterSconeOpenSim4()
	{
		GetModelFactory().register_type< ModelOpenSim4 >( "ModelOpenSim4" );
	}

	void ConvertModelOpenSim4( const xo::path& inputFile, const xo::path& outputFile )
	{
		OpenSim::Model m( inputFile.str() );
		m.print( outputFile.str() );
	}
}
