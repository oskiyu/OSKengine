#pragma once

#include "OSKmacros.h"

namespace OSK {

	class OSKAPI_CALL Kilograms {

	public:

		explicit Kilograms(float value) : value(value) {}

	private:

		float value = 0.0f;

	};

}