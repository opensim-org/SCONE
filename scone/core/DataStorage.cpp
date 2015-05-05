#include "stdafx.h"

#include "DataStorage.h"
#include "Exception.h"

namespace scone
{
	DataStorage::Frame::Frame( DataStorage& store, TimeInSeconds i_time ) :
	m_Store( store ),
	time( i_time ),
	data( store.GetLabelCount() )
	{
	}

	DataStorage::Frame::Frame( const Frame& other ) :
	m_Store( other.m_Store ),
	time( other.time ),
	data( other.data )
	{
	}

	DataStorage::Frame& DataStorage::Frame::operator=( const Frame& other )
	{
		SCONE_ASSERT( &other.m_Store == &m_Store );
		time = other.time;
		data = other.data;
		return *this;
	}

	void DataStorage::Frame::AddSample( const String& label, const ValueType& value )
	{
		Index idx = m_Store.GetLabelIndex( label );
		if ( idx >= data.size() )
			data.resize( idx + 1 );
		data[ idx ] = value;
	}

	DataStorage::DataStorage()
	{
	}

	DataStorage::~DataStorage()
	{
	}

	DataStorage::Frame& DataStorage::AddFrame( TimeInSeconds time )
	{
		SCONE_CONDITIONAL_THROW( time < m_Frames.back().time, "Frames must be added with increasing timestamps" );

		m_Frames.push_back( Frame( *this, time ) );
		return m_Frames.back();
	}

	size_t DataStorage::GetLabelIndex( const String& label )
	{
		auto label_iter = m_Labels.find( label );
		if ( label_iter == m_Labels.end() )
			label_iter = m_Labels.insert( std::make_pair( label, m_Labels.size() ) ).first;
		return label_iter->second;
	}

	size_t DataStorage::GetLabelCount()
	{
		return m_Labels.size();
	}
}
