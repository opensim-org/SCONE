#include "xo/filesystem/filesystem.h"
#include "scone/core/system_tools.h"
#include "xo/diagnose/test_framework.h"

void propnode_stream_test()
{
	xo::path rootpath = scone::GetFolder( scone::SCONE_ROOT_FOLDER );
	xo::path p1path = rootpath / "scenarios/f1020_walk_GH.xml";
	auto p1 = scone::load_file_with_include( xo::path( p1path.string() ) );

	auto p2path = xo::temp_directory_path() / p1path.filename();
	save_file( p1, xo::path( p2path.string() ), "xml" );
	
	auto p2 = scone::load_file_with_include( xo::path( p2path.string() ) );

	XO_TEST( p1 == p2 );
}
