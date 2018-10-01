/*
** HasSignature.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "types.h"
#include "PropNode.h"

namespace scone
{
	/// Class that has a signature, used for file output.
	class SCONE_API HasSignature
	{
	public:
		HasSignature( const PropNode& pn );
		virtual ~HasSignature();

		/// Prefix signature with custom string.
		String signature_prefix;

		/// Append custom string to signature.
		String signature_postfix;

		String GetSignature() const;

	protected:
		virtual String GetClassSignature() const { return ""; }

	private:
	};
}			
