#pragma once

#include "DynamicArray.hpp"
#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Incluye los valores de transform y timestamps de la animaci�n de un hueso,
	/// y c�mo interpolarlos.
	/// </summary>
	struct AnimationSampler {

		enum class InterpolationType {
			LINEAR,
			STEP,
			CUBICSPLINE
		};

		InterpolationType interpolationType = InterpolationType::LINEAR;

		/// <summary> Timestamps en los que se realiza alguna transformaci�n en la animaci�n. </summary>
		DynamicArray<TDeltaTime> timestamps;
		/// <summary> Valor final de la posici�n / rotaci�n escala de un bone en un timestamp.</summary>
		DynamicArray<Vector4f> outputs;

	};

}
