#pragma once

#include <optional>

namespace OSK {

	/// <summary>
	/// Contiene información sobre qué familias de comandos soporta una GPU.
	/// </summary>
	struct QueueFamilyIndices {

		/// <summary>
		/// Cola gráfica.
		/// </summary>
		std::optional<uint32_t> graphicsFamily;

		/// <summary>
		/// Cola de presentación.
		/// (Pueden no ser la misma).
		/// </summary>
		std::optional<uint32_t> presentFamily;

		/// <summary>
		/// ¿Hay alguna cola?
		/// </summary>
		/// <returns>True si hay alguna cola disponible.</returns>
		bool IsComplete() const;

	};

}
