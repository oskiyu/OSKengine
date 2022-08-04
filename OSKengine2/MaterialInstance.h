#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "UniquePtr.hpp"

#include "IMaterialSlot.h"

#include <string>

namespace OSK::GRAPHICS {

	class Material;
	class MaterialLayout;

	/// <summary>
	/// Mientras que un Material define a grandes rasgos el comportamiento al renderizarse los objetos,
	/// un MaterialInstance permite ajustar el renderizado más finamente, permitiendo asignar qué recursos
	/// se envían a los shaders (UNIFORM BUFFER, TEXTURE, etc...).
	/// </summary>
	class OSKAPI_CALL MaterialInstance {

	public:

		/// <summary>
		/// Crea una instancia del material.
		/// </summary>
		/// <param name="material">Material del que se crea la instancia.</param>
		MaterialInstance(Material* material);

		~MaterialInstance();

		/// <summary>
		/// Registra un slot.
		/// 
		/// @note TODOS los slots del Material (presentes en su layout) deben registrarse.
		/// </summary>
		/// <param name="name"></param>
		void RegisterSlot(const std::string& name);

		/// <summary>
		/// Devuelve el slot con el nombre dado.
		/// </summary>
		IMaterialSlot* GetSlot(const std::string& name) const;

		/// <summary>
		/// Devuelve el material del que es instancia esta instancia.
		/// </summary>
		Material* GetMaterial() const;

		/// <summary>
		/// Devuelve el layout de esta instancia (es decir, el layout del material padre).
		/// </summary>
		/// <returns></returns>
		const MaterialLayout* GetLayout() const;

	private:

		HashMap<std::string, UniquePtr<IMaterialSlot>> slots;
		Material* ownerMaterial = nullptr;

	};

}
