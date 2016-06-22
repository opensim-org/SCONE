#pragma once

#include <memory>

// class and unique pointer type forward declaration
#define SCONE_DECLARE_CLASS_AND_PTR( _class_ ) \
	class _class_; \
	typedef std::unique_ptr< _class_ > _class_##UP;

// struct and unique pointer type forward declaration
#define SCONE_DECLARE_STRUCT_AND_PTR( _class_ ) \
struct _class_; \
	typedef std::unique_ptr< _class_ > _class_##UP;
