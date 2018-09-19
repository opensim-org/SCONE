#pragma once

#include "Side.h"
#include "scone/core/HasName.h"

namespace scone
{
	class SCONE_API Location
	{
	public:
		Location( Side s = NoSide, bool m = false, bool sym = false ) : side( s ), mirrored( m ), symmetric( s ) {}

		Side side;
		bool mirrored; // TODO: check out if we need this!
		bool symmetric; // TODO: implement everywhere!

		String GetSidedName( const String& name ) const {
			return GetNameNoSide( name ) + GetSideName( GetSide() );
		}

		String GetParName( const String& name ) const {
			if ( symmetric )
				return GetNameNoSide( name );
			else return GetSidedName( name );
		}

		// TODO: see where this is used and if its needed or can be replaced with FindComponent
		String ConvertName( const String& name ) const
		{
			auto org_side = ::scone::GetSideFromName( name );
			auto noside_name = GetNameNoSide( name );
			if ( org_side != NoSide )
			{
				if ( mirrored )
					return scone::GetSidedName( noside_name, GetMirroredSide( org_side ) );
				else return name;
			}
			else // no side in initial name
			{
				return scone::GetSidedName( noside_name, GetSide() );
			}
		}

		//String GetName() const
		//{
		//	return GetFullSideName( side ) + ( mirrored ? "Mirrored" : "" );
		//}

		Side GetSide() const
		{
			return mirrored ? GetMirroredSide( side ) : side;
		}

		template< typename T > T& FindComponent( std::vector< T >& cont, const String& name )
		{
			auto name_side = GetSideFromName( name );
			if ( name_side == NoSide )
			{
				if ( auto it = TryFindByName( cont, name ); it != cont.end() )
					return *it;
				else ( auto it = TryFindByName( cont, GetSidedName( name ) ); it != cont.end() )
					std::find_if( cont.begin(), cont.end(), [&]( T& item ) { return item->GetName() == name + GetSideName( side ); } );
			}
			else 
			if ( it == cont.end() )
				SCONE_THROW( "Could not find " + name + " or " + name + GetSideName( side ) );

			return *it;
		}


	};

	// TODO: check if we need this!
	inline Location MakeMirrored( Location a ) { a.mirrored = !a.mirrored;	return a; }
}
