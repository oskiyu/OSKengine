#pragma once

#include "DynamicArray.hpp"
#include "SdfDrawCall2D.h"

namespace OSK::GRAPHICS {

	class Material;


	/// @brief Conjunto de llamadas de renderizado
	/// que usan un mismo material y se pueden
	/// juntar en una �nica llamada de renderizado.
	struct SdfBindlessBatch2D {

		/// @brief Llamadas individuales.
		DynamicArray<SdfDrawCall2D> drawCalls{};

		/// @brief Material.
		/// @note No ser� null.
		const Material* material = nullptr;

	};

}
