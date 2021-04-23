#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

#include "Vertex.h"

#include <vector>

namespace OSK {

	/// <summary>
	/// Almacena los vértices e índices de un modelo.
	/// </summary>
	struct OSKAPI_CALL TempModelData {

		/// <summary>
		/// Vértices.
		/// </summary>
		std::vector<Vertex> Vertices;

		/// <summary>
		/// Índices.
		/// </summary>
		std::vector<vertexIndex_t> Indices;

	};

}