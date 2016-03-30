#include "Profiler.h"
#include "tools.h"

#include <algorithm>

namespace scone
{
	Profiler& Profiler::GetGlobalInstance()
	{
		// this should be thread-safe in c++14
		// TODO: use thread local storage, or ensure thread safety
		static Profiler g_GlobalInstance;
		return g_GlobalInstance;
	}

	ScopedProfile::ScopedProfile( class Profiler& prof, const String& scope ) :
	m_Profiler( prof )
	{
		if ( m_Profiler.IsActive() )
			m_StartTime = m_Profiler.StartMeasure( scope );
	}

	ScopedProfile::~ScopedProfile()
	{
		if ( m_Profiler.IsActive() )
			m_Profiler.StopMeasure( m_StartTime );
	}

	Profiler::Profiler() :
	m_Root( nullptr ),
	m_Current( &m_Root ),
	m_bActive( true )
	{

	}

	Profiler::~Profiler()
	{

	}

	HighResolutionTime Profiler::StartMeasure( const String& scope )
	{
		m_Current = &m_Current->GetOrAddChild( scope );
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );
		return li.QuadPart;
	}

	void Profiler::StopMeasure( HighResolutionTime start_time )
	{
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );
		m_Current->AddSample( li.QuadPart - start_time );
		m_Current = m_Current->parent;
	}

	PropNode Profiler::GetReport()
	{
		PropNode pn;
		m_Root.GetReport( pn );
		return pn;
	}

	void Profiler::Activate()
	{
		m_bActive = true;
	}

	void Profiler::Suspend()
	{
		m_bActive = false;
	}

	bool Profiler::IsActive()
	{
		return m_bActive;
	}

	void Profiler::Reset()
	{
		SCONE_ASSERT_MSG( m_Current == &m_Root, "Cannot reset while a ScopedProfile is active" );
		m_Root.children.clear();
	}

	Profiler::Item::Item( Item* p ) :
	parent( p ),
	num_samples( 0 ),
	inclusive_time( 0 ),
	peak_time( 0 )
	{
	}

	void Profiler::Item::AddSample( HighResolutionTime cur_time )
	{
		++num_samples;
		inclusive_time += cur_time;
		peak_time = std::max( peak_time, cur_time );
	}

	Profiler::Item& Profiler::Item::GetOrAddChild( const String& scope )
	{
		auto it = children.find( scope );
		if ( it == children.end() )
		{
			// create new item
			children[ scope ] = std::unique_ptr< Item >( new Item( this ) );
			it = children.find( scope );
		}
		return *it->second;
	}

	HighResolutionTime Profiler::Item::GetReport( PropNode& pn )
	{
		Item* topnode = this;
		for (; topnode->parent && topnode->parent->parent; topnode = topnode->parent );
		HighResolutionTime topnode_time = topnode->inclusive_time;
		HighResolutionTime children_time = 0;

		// TODO: sort children first
		for ( auto it = children.begin(); it != children.end(); ++it )
			children_time += it->second->GetReport( pn.AddChild( it->first ) );

		pn.SetValueType( stringf( "%6.2f (%5.2f exclusive)", 100.0 * inclusive_time / topnode_time, 100.0 * ( inclusive_time - children_time ) / topnode_time ) );

		return inclusive_time;
	}
}
