#pragma once

#include "Exception.h"
#include "PropNode.h"
#include "types.h"

namespace scone
{
	struct FactoryProps {
		const PropNode& props() const { SCONE_THROW_IF( !props_, "Invalid Factory Properties" ); return *props_; }
		const String& type() const { SCONE_THROW_IF( !props_, "Invalid Factory Properties" ); return type_; }
		operator bool() const { return props_; }
		String type_;
		const PropNode* props_ = nullptr;
	};
}
