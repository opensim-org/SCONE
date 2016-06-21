#pragma once

#include <memory>

// class and shared pointer forward declaration macro
#define SCONE_DECLARE_CLASS_AND_PTR( _class_ ) \
	class _class_; \
	typedef std::shared_ptr< _class_ > _class_##SP; \
	typedef std::unique_ptr< _class_ > _class_##UP;

// class and shared pointer forward declaration macro
#define SCONE_DECLARE_STRUCT_AND_PTR( _class_ ) \
struct _class_; \
	typedef std::shared_ptr< _class_ > _class_##SP; \
	typedef std::unique_ptr< _class_ > _class_##UP;
