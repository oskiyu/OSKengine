#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"
#include "OwnedPtr.h"

#include <string>

namespace OSK::GRAPHICS {

	class Material;
	class MaterialLayout;
	class IMaterialSlot;

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
		MaterialInstance(const Material* material);

		/// <summary>
		/// Registra un slot.
		/// TODOS los slots del Material (presentes en su layout) deben registrarse.
		/// </summary>
		/// <param name="name"></param>
		void RegisterSlot(const std::string& name);

		/// <summary>
		/// Devuelve el slot con el nombre dado.
		/// </summary>
		IMaterialSlot* GetSlot(const std::string& name);

		/// <summary>
		/// Devuelve el material del que es instancia esta instancia.
		/// </summary>
		const Material* GetMaterial() const;

		/// <summary>
		/// Devuelve el layout de esta instancia (es decir, el layout del material padre).
		/// </summary>
		/// <returns></returns>
		const MaterialLayout* GetLayout() const;

	private:

		HashMap<std::string, OwnedPtr<IMaterialSlot>> slots;
		const Material* ownerMaterial;

	};

}
