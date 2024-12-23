#pragma once

#include "TransformComponent2D.h"
#include "GameObject.h"
#include "Color.hpp"

#include <string_view>


namespace OSK::ASSETS {
	struct FontInstance;
}

namespace OSK::GRAPHICS {

	/// @brief Información para el renderizado
	/// de texto 2D.
	struct SdfStringInfo {

		/// @brief Texto a renderizar.
		std::string_view text;

		/// @brief Instancia de la fuente a usar
		/// para el renderizado.
		/// @pre No debe ser null.
		const ASSETS::FontInstance* font = nullptr;

		/// @brief Transform inicial del texto.
		/// Será el transform de la primera letra del texto.
		ECS::Transform2D transform = ECS::Transform2D(ECS::GameObjectIndex::CreateEmpty());

		/// @brief Color del texto.
		Color color = Color::White;

	};

}
