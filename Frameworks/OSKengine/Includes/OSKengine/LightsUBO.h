#pragma once

#include "OSKmacros.h"

#include <vulkan/vulkan.h>

#include "PointLight.h"
#include "DirectionalLight.h"

#include <vector>
#include "GpuDataBuffer.h"

namespace OSK {

	/// <summary>
	/// Interfaz con un UBO que contiene informaci�n sobre las luces.
	/// </summary>
	class OSKAPI_CALL LightUBO {

	public:

		/// <summary>
		/// Luz direccional.
		/// </summary>
		DirectionalLight directional{};

		/// <summary>
		/// Luces puntuales.
		/// </summary>
		std::vector<PointLight> points{};

		/// <summary>
		/// Obtiene el tama�o, en bytes, de todas las luces puntuales juntas.
		/// </summary>
		/// <returns>Tama�o total.</returns>
		inline const size_t PointsSize() const {
			return points.size() * sizeof(PointLight);
		}

		/// <summary>
		/// Obtiene el tama�o, en bytes, del UBO.
		/// </summary>
		/// <returns>Obtiene el tama�o, en bytes, del UBO.</returns>
		inline const size_t Size() const {
			return sizeof(DirectionalLight) + PointsSize();
		}

		/// <summary>
		/// Actualiza el UBO.
		/// </summary>
		/// <param name="logicalDevice">Logical device del renderizador.</param>
		/// <param name="buffer">Buffer del ubo.</param>
		void UpdateBuffer(VkDevice logicalDevice, GpuDataBuffer& buffer);

	};

}