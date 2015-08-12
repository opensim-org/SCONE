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
#define SCONE_ASSERT_DEBUG( expression ) \
		if (!(expression)) throw scone::RuntimeException( "Assertion Failure in " + std::string( __func__ ) + "(): "#expression );
#else
#define SCONE_ASSERT_DEBUG( expression )
#endif

#define SCONE_ASSERT( expression ) \
	if (!(expression)) throw scone::RuntimeException( "Assertion Failure in " + std::string( __func__ ) + "(): "#expression );

#define SCONE_ASSERT_MSG( expression, message ) \
	if (!(expression)) throw scone::RuntimeException( "Assertion Failure in " + std::string( __func__ ) + "(): "#expression" (" + String( message ) + ")" );

#define SCONE_THROW( message ) throw scone::RuntimeException(std::string( __func__ ) + "(): " + std::string( message ) )
#define SCONE_THROW_IF( _condition_, _message_ ) \
    do { \
        if ( _condition_ ) throw scone::RuntimeException( std::string( __func__ ) + "(): " + std::string( _message_ ) ); } while (false);
