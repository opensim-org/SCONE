#include "stdafx.h"
#include "Profiler.h"
#include "tools.h"

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
			m_Time = m_Profiler.StartMeasure( scope );
	}

	ScopedProfile::~ScopedProfile()
	{
		if ( m_Profiler.IsActive() )
			m_Profiler.StopMeasure( m_Time );
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

	long long Profiler::StartMeasure( const String& scope )
	{
		m_Current = &m_Current->GetOrAddChild( scope );
		LARGE_INTEGER li;
		QueryPerformanceCounter( &li );
		return li.QuadPart;
	}

	void Profiler::StopMeasure( long long start_time )
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

	Profiler::Item::Item( Item* p ) :
	parent( p ),
	num_samples( 0 ),
	inclusive_time( 0 ),
	peak_time( 0 )
	{
	}

	void Profiler::Item::AddSample( long long cur_time )
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

	long long Profiler::Item::GetReport( PropNode& pn )
	{
		Item* topnode = this;
		for (; topnode->parent && topnode->parent->parent; topnode = topnode->parent );
		long long topnode_time = topnode->inclusive_time;
		long long children_time = 0;

		// TODO: sort children first
		for ( auto it = children.begin(); it != children.end(); ++it )
			children_time += it->second->GetReport( pn.AddChild( it->first ) );

		pn.SetValue( GetStringF( "%.2f (%.2f exclusive)", 100.0 * inclusive_time / topnode_time, 100.0 * ( inclusive_time - children_time ) / topnode_time ) );

		return inclusive_time;
	}
}
