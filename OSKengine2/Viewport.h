#pragma once

#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// El viewport define que área de la textura final será renderizada.
	/// </summary>
	struct Viewport {

		/// <summary>
		/// Área que se renderizará.
		/// </summary>
		Vector4ui rectangle{};

		/// <summary>
		/// Profundidad mínima que se renderizará.
		/// Algo que esté por detrás no aparecerá.
		/// </summary>
		float minDepth = 0.0f;

		/// <summary>
		/// Profundidad máxima que se renderizará.
		/// Algo que esté por delante no aparecerá.
		/// </summary>
		float maxDepth = 1.0f;

	};

}
