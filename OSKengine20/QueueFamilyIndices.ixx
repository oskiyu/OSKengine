#include <optional>
#include <cstdint>

export module OSKengine.Graphics.QueueFamilyIndices;

export namespace OSK {

	/// <summary>
	/// Contiene informaci�n sobre qu� familias de comandos soporta una GPU.
	/// </summary>
	struct QueueFamilyIndices {

		/// <summary>
		/// Cola gr�fica.
		/// </summary>
		std::optional<uint32_t> graphicsFamily;

		/// <summary>
		/// Cola de presentaci�n.
		/// (Pueden no ser la misma).
		/// </summary>
		std::optional<uint32_t> presentFamily;

		/// <summary>
		/// �Hay alguna cola?
		/// </summary>
		/// <returns>True si hay alguna cola disponible.</returns>
		bool IsComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}

	};

}
