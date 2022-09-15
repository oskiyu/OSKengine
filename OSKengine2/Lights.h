#pragma once

#include "Color.hpp"
#include "Vector4.hpp"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Representa una luz direccional.
	/// </summary>
	struct DirectionalLight {

		/// <summary>
		/// Dirección de la luz.
		/// </summary>
		alignas(16) Vector4f directionAndIntensity;

		/// <summary>
		/// Color de la luz.
		/// </summary>
		alignas(16) Color color;

	};

	/// <summary>
	/// Representa una luz puntual.
	/// </summary>
	struct PointLight {

		/// <summary>
		/// Posición de la luz.
		/// </summary>
		alignas(16) glm::vec3 position;

		/// <summary>
		/// Color.
		/// </summary>
		alignas(16) glm::vec4 color;

		/// <summary>
		/// Información de la luz: <para/>
		/// x = Intensidad. <para/>
		/// y = Radio. <para/>
		/// </summary>
		alignas(16) glm::vec4 information;

		/// <summary>
		/// Establece la intensidad de la luz.
		/// </summary>
		void SetIntensity(float intensity) {
			information.x = intensity;
		}

		/// <summary>
		/// Establece el radio a la que llega la luz.
		/// </summary>
		void SetRadius(float radius) {
			information.y = radius;
		}

	};

	/// <summary>
	/// Estructura preparada para ser insertada en un storage buffer.
	/// </summary>
	/// 
	/// @todo Implementación.
	struct LightsBufferStruct {

		alignas(16) TSize numPointLights = 0;

		DynamicArray<PointLight> pointLights;

	};

}
