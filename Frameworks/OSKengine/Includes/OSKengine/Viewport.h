#pragma once

#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// El viewport define que �rea de la textura final ser� renderizada.
	/// </summary>
	struct Viewport {

		/// <summary>
		/// �rea que se renderizar�.
		/// </summary>
		Vector4ui rectangle{};

		/// <summary>
		/// Profundidad m�nima que se renderizar�.
		/// Algo que est� por detr�s no aparecer�.
		/// </summary>
		float minDepth = 0.0f;

		/// <summary>
		/// Profundidad m�xima que se renderizar�.
		/// Algo que est� por delante no aparecer�.
		/// </summary>
		float maxDepth = 1.0f;

	};

}
