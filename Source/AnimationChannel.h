#pragma once

#include "NumericTypes.h"

namespace OSK::GRAPHICS {

	struct AnimationSampler;

	/// <summary> Información sobre el valor de una transformación individual de la animación. </summary>
	struct AnimationChannel {

		enum class ChannelType {
			TRANSLATION,
			ROTATION,
			SCALE
		};

		/// <summary> Posición / rotación / escala. </summary>
		ChannelType type = ChannelType::TRANSLATION;
		/// <summary> Hueso al que se aplica. </summary>
		UIndex32 nodeId = 0;
		/// <summary> Sampler usado. </summary>
		UIndex32 samplerIndex = 0;

	};

}
