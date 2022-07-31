#pragma once

#include "Color.hpp"
#include "Vector3.hpp"
#include "DynamicArray.hpp"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Representa una luz direccional.
	/// </summary>
	struct DirectionalLight {

		/// <summary>
		/// Direcci�n de la luz.
		/// </summary>
		alignas(16) Vector3 direction;

		/// <summary>
		/// Color de la luz.
		/// </summary>
		alignas(16) Color color;

		/// <summary>
		/// Intensidad de la luz. 
		/// (0.0 - 1.0)
		/// </summary>
		alignas(16) float intensity = 1.0f;

	};

	/// <summary>
	/// Representa una luz puntual.
	/// </summary>
	struct PointLight {

		/// <summary>
		/// Posici�n de la luz.
		/// </summary>
		alignas(16) glm::vec3 position;

		/// <summary>
		/// Color.
		/// </summary>
		alignas(16) glm::vec4 color;

		/// <summary>
		/// Informaci�n de la luz: <para/>
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
	/// @todo Implementaci�n.
	struct LightsBufferStruct {

		alignas(16) TSize numPointLights = 0;

		DynamicArray<PointLight> pointLights;

	};

}
