#pragma once

#include "ApiCall.h"

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Color.hpp"

#include "glm/fwd.hpp"


namespace OSK::GRAPHICS {

	struct SdfDrawCall2D;


	/// @brief Contenido para una instancia renderizada por
	/// el renderizador Signed Field Distance 2D Bind-less.
	struct OSKAPI_CALL SdfBindlessBufferContent2D {

		/// @param drawCall Información del renderizado.
		/// @return Información procesada para insertarse
		/// directamente en el buffer de la GPU.
		static SdfBindlessBufferContent2D From(const SdfDrawCall2D& drawCall);

		/// @brief Rellena la estructura con la información
		/// de renderizado indicada.
		/// @param drawCall Información de renderizado.
		void Fill(const SdfDrawCall2D& drawCall);


		/// @brief 
		/// x: shape
		/// y: contentType
		/// z: fill
		alignas(16) Vector3i typeInfo {};

		alignas(16) Vector4f textureCoords {};

		alignas(16) Color mainColor {};
		alignas(16) Color secondaryColor {};

		/// @brief 
		/// x: Center position.x
		/// y: Center position.y
		/// z: border width
		/// W: padding
		alignas(16) Vector4f floatInfo {};

		alignas(16) glm::mat4 matrix = glm::mat4(1.0f);

	};

}
