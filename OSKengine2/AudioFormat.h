#pragma once

namespace OSK::AUDIO {

	/// @brief Formato de un audio.
	enum class Format {

		/// @brief Audio de un solo canal con 8 bits por sample.
		MONO8,

		/// @brief Audio de un solo canal con 16 bits por sample.
		MONO16,

		/// @brief Audio de dos canales con 8 bits por sample.
		STEREO8,

		/// @brief Audio de dos canales con 16 bits por sample.
		STEREO16

	};

}
