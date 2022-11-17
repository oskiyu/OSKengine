#pragma once

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
		ChannelType type;
		/// <summary> Hueso al que se aplica. </summary>
		TIndex nodeId = 0;
		/// <summary> Sampler usado. </summary>
		TIndex samplerIndex = 0;

	};

}
