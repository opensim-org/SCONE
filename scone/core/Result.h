#pragma once

namespace scone
{
	template< CodeT = int >
	struct Result
	{
		virtual operator bool() const = 0;
		virtual const char* message() const = 0;
		virtual CodeT code() const = 0;
	};

	template< CodeT = int >
	struct Success : Result< CodeT >
	{
		virtual operator bool() const override { return true; }
		virtual const char* message() const override { return ""; }
		virtual CodeT code() const override { return CodeT( 0 ); }
	};

	template< CodeT = int >
	struct Failure : Result< CodeT >
	{
		Failure( CodeT code, String message = "" ) : code_( code ), msg_( message ) { }
		virtual operator bool() const { return false; }
		virtual const char* message() const override { return msg_.c_str(); }
		virtual CodeT code() const override { return code_; }

	private:
		CodeT code_;
		String msg_;
	};
}
