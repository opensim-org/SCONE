/*
** Dof.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Dof.h"

#pragma warning( disable: 4355 )

namespace scone
{
	Dof::Dof( Joint& j ) : m_Joint( j ) {}
	Dof::~Dof() {}
}
