#pragma once

#include <stdexcept>
#include <iostream>

namespace scone
{
	struct RuntimeException : public std::runtime_error {
		RuntimeException(const std::string& msg) : runtime_error(msg) { std::cout << "scone::RuntimeException: " << msg << std::endl; }
	};

	struct LogicException : public std::logic_error {
		LogicException(const std::string& msg) : logic_error(msg) { std::cout << "scone::LogicException: " << msg << std::endl; }
	};
}

// assertion and exceptions
#define SCONE_THROW_NOT_IMPLEMENTED throw scone::LogicException(""__FUNCTION__" not implemented");

#define SCONE_ASSERT_ALWAYS

#if defined(_DEBUG) || defined(SCONE_ASSERT_ALWAYS)
#define SCONE_ASSERT( expression ) \
	if (!(expression)) throw scone::LogicException("Assertion Failure: "#expression" in function "__FUNCTION__"");
#else
#define SCONE_ASSERT( expression )
#endif

#define SCONE_VALIDATE( expression ) \
	if (!(expression)) throw scone::RuntimeException("Assertion Failure: "#expression" in function "__FUNCTION__"");

#define SCONE_THROW( message ) throw scone::RuntimeException(std::string(""__FUNCTION__": ") + (message) )
