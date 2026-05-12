#pragma once

#include "ApiCall.h"
#include "Format.h"
#include "DynamicArray.hpp"

#include "RenderpassVk.h"
#include "HashMap.hpp"

#include <optional>

namespace OSK::GRAPHICS {

	/// @brief Entrada con información sobre
	/// un attachment/imagen en concreto.
	struct RenderpassVkMapEntryPerAttachment {

		/// @brief Formato de la imagen.
		Format format = Format::UNKNOWN;

		/// @brief En caso de ser color/depth target,
		/// indica si debe ser limpiado antes de comenzar
		/// el renderizado.
		bool clear = false;

		bool operator==(const RenderpassVkMapEntryPerAttachment& other) const = default;

	};


	/// @brief Entrada para un renderpass.
	/// Depende de los formatos de las imágenes.
	struct RenderpassVkMapEntry {

		/// @brief Entradas para los targets de color.
		DynamicArray<RenderpassVkMapEntryPerAttachment> colors;

		/// @brief Entrada para el tarjet de profundidad
		/// si existe.
		std::optional<RenderpassVkMapEntryPerAttachment> depth;

		bool operator==(const RenderpassVkMapEntry& other) const;

	};

}

template<> struct std::hash<OSK::GRAPHICS::RenderpassVkMapEntry> {
	std::size_t operator()(const OSK::GRAPHICS::RenderpassVkMapEntry& entry) const {
		return std::hash<std::size_t>()(entry.colors.GetSize()); // TODO: improve
	}
};

namespace OSK::GRAPHICS {

	class GpuVk;

	/// @brief Contiene los renderpasses a usar, dependiendo
	/// de las imágenes sobre las que se renderice.
	class OSKAPI_CALL RenderpassVkMap {

	public:

		/// @brief Obtiene el renderpass necesario según la
		/// información dada.
		/// 
		/// En caso de que no existiera, se crea.
		/// @param entry Información sobre el renderizado.
		RenderpassVk* GetRenderpass(const GpuVk* gpu, const RenderpassVkMapEntry& entry);

		/// @brief Elimina los renderpasses.
		void Clear();

	private:

		std::unordered_map<RenderpassVkMapEntry, RenderpassVk> m_map;

	};

}
