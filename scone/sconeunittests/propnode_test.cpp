#include "common.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

BOOST_AUTO_TEST_CASE( propnode_stream_test )
{
	path rootpath = scone::GetFolder( scone::SCONE_ROOT_FOLDER ).str();
	path p1path = rootpath / "scenarios/f1020_walk_GH.xml";
	auto p1 = scone::load_file_with_include( flut::path( p1path.string() ) );

	auto p2path = temp_directory_path() / p1path.filename();
	save_xml( p1, flut::path( p2path.string() ) );
	
	auto p2 = scone::load_file_with_include( flut::path( p2path.string() ) );

	BOOST_CHECK( p1 == p2 );
}
