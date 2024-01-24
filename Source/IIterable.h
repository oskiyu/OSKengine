// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"

namespace OSK {

	class OSKAPI_CALL IIterator {

	};

	class OSKAPI_CALL IIterable {

	public:

		virtual IIterator begin() const noexcept = 0;
		virtual IIterator end() const noexcept = 0;

	};

}
