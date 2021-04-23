#pragma once

#include "OSKmacros.h"
#include "Vertex.h"

#include <array>

namespace OSK {

	/// <summary>
	/// Contiene los huesos a los que está ligado un vértice.
	/// </summary>
	struct OSKAPI_CALL VertexBoneData {

		/// <summary>
		/// IDs de los huesos de un vértice.
		/// </summary>
		std::array<uint32_t, OSK_ANIM_MAX_BONES_PER_VERTEX> IDs;

		/// <summary>
		/// Pesos de los huesos.
		/// </summary>
		std::array<float_t, OSK_ANIM_MAX_BONES_PER_VERTEX> Weights;

		/// <summary>
		/// Liga un vértice con un hueso.
		/// </summary>
		/// <param name="id">ID del hueso.</param>
		/// <param name="weight">Pseo del hueso.</param>
		void Add(uint32_t id, float_t weight);

	};

}