#pragma once

#include "OSKmacros.h"
#include "OSKtypes.h"

#include "Vertex.h"

#include <vector>

namespace OSK {

	/// <summary>
	/// Almacena los v�rtices e �ndices de un modelo.
	/// </summary>
	struct OSKAPI_CALL TempModelData {

		/// <summary>
		/// V�rtices.
		/// </summary>
		std::vector<Vertex> vertices;

		/// <summary>
		/// �ndices.
		/// </summary>
		std::vector<vertexIndex_t> indices;

	};

}