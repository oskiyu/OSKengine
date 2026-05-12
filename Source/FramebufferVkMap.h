#pragma once

#include "ApiCall.h"
#include "DynamicArray.hpp"

#include "FramebufferVk.h"
#include "HashMap.hpp"
#include <span>

namespace OSK::GRAPHICS {

	class GpuImageViewVk;
	class GpuVk;
	class RenderpassVk;

	/// @brief Entrada con las vistas que debe tener
	/// un frambebuffer en concreto.
	struct FramebufferVkMapEntry {
		DynamicArray<const GpuImageViewVk*> views;

		bool operator==(const FramebufferVkMapEntry& other) const;
	};

}

template<> struct std::hash<OSK::GRAPHICS::FramebufferVkMapEntry> {
	std::size_t operator()(const OSK::GRAPHICS::FramebufferVkMapEntry& entry) const {
		return std::hash<std::size_t>()(entry.views.GetSize()); // TODO: improve
	}
};

namespace OSK::GRAPHICS {

	/// @brief Para un renderpass en concreto, devuelve
	/// el framebuffer que tenga enlazadas las vistas
	/// finales sobre las que se vaya a renderizar.
	class OSKAPI_CALL FramebufferVkMap {

	public:

		explicit FramebufferVkMap(const GpuVk* gpu);

		/// @brief Devuelve el framebuffer usable sobre
		/// las imágenes @p entry.
		/// @param renderpass Renderpass dueño del framebuffer.
		FramebufferVk* GetFramebuffer(const FramebufferVkMapEntry& entry, const RenderpassVk& renderpass);

	private:

		std::unordered_map<FramebufferVkMapEntry, FramebufferVk> m_map;

		const GpuVk* m_gpu = nullptr;

	};

}
