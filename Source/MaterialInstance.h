#pragma once

#include "ApiCall.h"
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

		OSK_DISABLE_COPY(MaterialInstance);
		OSK_DEFAULT_MOVE_OPERATOR(MaterialInstance);

		/// <summary>
		/// Registra un slot.
		/// 
		/// @note TODOS los slots del Material (presentes en su layout) deben registrarse.
		/// </summary>
		/// <param name="name"></param>
		/// 
		/// @throws DescriptorPoolCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws DescriptorLayoutCreationException Si hay algún error nativo durante la creación del material slot.
		/// @throws MaterialSlotCreationException Si hay algún error durante la creación del material slot.
		void RegisterSlot(const std::string& name);

		/// Devuelve el slot con el nombre dado.
		const IMaterialSlot* GetSlot(std::string_view name) const { return slots.find(name)->second.GetPointer(); }
		IMaterialSlot* GetSlot(std::string_view name) { return slots.find(name)->second.GetPointer(); }

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

		std::unordered_map<std::string, UniquePtr<IMaterialSlot>, StringHasher, std::equal_to<>> slots;
		Material* ownerMaterial = nullptr;

	};

}
