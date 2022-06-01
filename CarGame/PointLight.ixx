module;

#include <OSKengine/Color.hpp>
#include <OSKengine/Vector3.hpp>

export module PointLight;

using namespace OSK;

/// <summary>
/// Representa una luz direccional.
/// </summary>
export struct DirectionalLight {

	/// <summary>
	/// Dirección de la luz.
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
	alignas(16) float intensity;

};
