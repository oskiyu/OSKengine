#pragma once

#include "DynamicArray.hpp"
#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Incluye los valores de transform y timestamps de la animación de un hueso,
	/// y cómo interpolarlos.
	/// </summary>
	struct AnimationSampler {

		enum class InterpolationType {
			LINEAR,
			STEP,
			CUBICSPLINE
		};

		InterpolationType interpolationType = InterpolationType::LINEAR;

		/// <summary> Timestamps en los que se realiza alguna transformación en la animación. </summary>
		DynamicArray<TDeltaTime> timestamps;
		/// <summary> Valor final de la posición / rotación escala de un bone en un timestamp.</summary>
		DynamicArray<Vector4f> outputs;

	};

}
