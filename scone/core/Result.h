#pragma once

namespace scone
{
	template< typename T >
	struct Result
	{
		virtual operator bool() const = 0;
		virtual const char* message() const = 0;
		virtual T code() const { return code_; }
	private:
		T code_;
	};

	template< typename T >
	struct Success : Result< T >
	{
		Success( const T& code ) : code_( code ) {}
		virtual operator bool() const override { return true; }
		virtual const char* message() const override { return ""; }
	};

	template< typename T >
	struct Failure : Result< CodeT >
	{
		Failure( const T& code, String message = "" ) : code_( code ), msg_( message ) { }
		virtual operator bool() const { return false; }
		virtual const char* message() const override { return msg_.c_str(); }
		virtual CodeT code() const override { return code_; }
	private:
		String msg_;
	};
}
