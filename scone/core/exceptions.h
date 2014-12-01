#pragma once
#include <stdexcept>

namespace scone
{
	struct RuntimeException : public std::runtime_error {
		RuntimeException(const std::string& msg) : runtime_error(msg) { };
	};

	struct LogicException : public std::logic_error {
		LogicException(const std::string& msg) : logic_error(msg) { };
	};
}


// assertion and exceptions
#define SCONE_THROW_NOT_IMPLEMENTED throw scone::LogicException(""__FUNCTION__" not implemented");

#ifdef _DEBUG
#define SCONE_ASSERT( expression ) \
	if (!(expression)) throw scone::RuntimeException("Assertion Failure: "#expression" in function "__FUNCTION__"");
#else
#define SCONE_ASSERT( expression )
#endif

#define SCONE_VALIDATE( expression ) \
	if (!(expression)) throw scone::RuntimeException("Assertion Failure: "#expression" in function "__FUNCTION__"");

#define SCONE_THROW( message ) throw scone::RuntimeException(std::string(""__FUNCTION__": ") + (message) )


