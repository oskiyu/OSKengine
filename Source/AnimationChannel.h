#pragma once

#include "NumericTypes.h"

namespace OSK::GRAPHICS {

	struct AnimationSampler;

	/// <summary> Informaci�n sobre el valor de una transformaci�n individual de la animaci�n. </summary>
	struct AnimationChannel {

		enum class ChannelType {
			TRANSLATION,
			ROTATION,
			SCALE
		};

		/// <summary> Posici�n / rotaci�n / escala. </summary>
		ChannelType type = ChannelType::TRANSLATION;
		/// <summary> Hueso al que se aplica. </summary>
		UIndex32 nodeId = 0;
		/// <summary> Sampler usado. </summary>
		UIndex32 samplerIndex = 0;

	};

}
