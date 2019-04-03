/*
** Exception.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <stdexcept>
#include <iostream>

#if defined(_MSC_VER)
#define __func__ __FUNCTION__
#endif

namespace scone
{
	struct RuntimeException : public std::runtime_error {
		RuntimeException(const std::string& msg) : runtime_error(msg) { }; // { std::cout << "scone::RuntimeException in " << msg << std::endl; }
	};

	struct LogicException : public std::logic_error {
		LogicException(const std::string& msg) : logic_error(msg) { }; // { std::cout << "scone::LogicException in " << msg << std::endl; }
	};
}

// assertion and exceptions
#define SCONE_THROW_NOT_IMPLEMENTED throw scone::LogicException( std::string( __func__ ) + "(): Function not implemented");

#define SCONE_ASSERT_DEBUG_ALWAYS

#if defined(_DEBUG) || defined(SCONE_ASSERT_DEBUG_ALWAYS)
#define SCONE_ASSERT_DEBUG( _expression_ ) \
		if (!(_expression_)) throw scone::RuntimeException( "Assertion Failure in " + std::string( __func__ ) + "(): "#_expression_ );
#else
#define SCONE_ASSERT_DEBUG( _expression_ )
#endif

#define SCONE_ASSERT( _expression_ ) \
	if (!(_expression_)) throw scone::RuntimeException( "Assertion Failure in " + std::string( __func__ ) + "(): "#_expression_ );

#define SCONE_ASSERT_MSG( _expression_, message ) \
	if (!(_expression_)) throw scone::RuntimeException( "Assertion Failure in " + std::string( __func__ ) + "(): "#_expression_" (" + String( message ) + ")" );

#define SCONE_THROW( message ) throw scone::RuntimeException(std::string( __func__ ) + "(): " + std::string( message ) )
#define SCONE_THROW_IF( _condition_, _message_ ) \
    do { \
        if ( _condition_ ) throw scone::RuntimeException( std::string( __func__ ) + "(): " + std::string( _message_ ) ); } while (false);

#define SCONE_CHECK_RANGE( _value_, _min_, _max_ ) \
	if ( _value_ < _min_ || _value_ > _max_ ) throw scone::RuntimeException( #_value_" must be a value between " + to_str( _min_ ) + " and " + to_str( _max_ ) );