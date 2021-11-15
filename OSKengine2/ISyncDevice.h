#pragma once

#include "OSKmacros.h"

namespace OSK {

	class OSKAPI_CALL ISyncDevice {

	public:

		~ISyncDevice() = default;

		template <typename T> T* As() const {
			return (T*)this;
		}

	};

}
