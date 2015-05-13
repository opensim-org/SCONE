#include "stdafx.h"
#include "Profiler.h"
#include "tools.h"

namespace scone
{

	ScopedProfile::ScopedProfile( class Profiler& prof, const String& scope ) :
	m_Profiler( prof )
	{
		m_Time = m_Profiler.StartMeasure( scope );
	}

	ScopedProfile::~ScopedProfile()
	{
		m_Profiler.StopMeasure( m_Time );
	}

	Profiler::Profiler() :
	m_Root( nullptr ),
	m_Current( &m_Root )
	{

	}

	Profiler::~Profiler()
	{

	}

	scone::TimeInSeconds Profiler::StartMeasure( const String& scope )
	{
		m_Current = &m_Current->GetOrAddChild( scope );
		return m_Timer.GetTime();
	}

	void Profiler::StopMeasure( TimeInSeconds start_time )
	{
		TimeInSeconds time = m_Timer.GetTime();
		m_Current->AddSample( time - start_time );
		m_Current = m_Current->parent;
	}

	PropNode Profiler::GetReport()
	{
		PropNode pn;
		m_Root.GetReport( pn );
		return pn;
	}

	Profiler::Item::Item( Item* p ) :
	parent( p ),
	num_samples( 0 ),
	inclusive_time( 0.0 ),
	peak_time( 0.0 )
	{
	}

	void Profiler::Item::AddSample( TimeInSeconds cur_time )
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

	TimeInSeconds Profiler::Item::GetReport( PropNode& pn )
	{
		Item* topnode = this;
		for (; topnode->parent && topnode->parent->parent; topnode = topnode->parent );
		TimeInSeconds topnode_time = topnode->inclusive_time;
		TimeInSeconds children_time = 0.0;

		for ( auto it = children.begin(); it != children.end(); ++it )
			children_time += it->second->GetReport( pn.AddChild( it->first ) );

		pn.SetValue( GetStringF( "%.2f (%.2f exclusive)", 100 * inclusive_time / topnode_time, 100 * ( inclusive_time - children_time ) / topnode_time ) );

		return inclusive_time;
	}
}
