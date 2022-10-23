#pragma once

#include "Component.h"
#include "OwnedPtr.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include <string>

namespace OSK {
	namespace ASSETS {
		class Model3D;
		class Texture;
	}
	namespace GRAPHICS {
		class Material;
		class MaterialInstance;
		class GpuImage;
		class IGpuUniformBuffer;
	}
}

namespace OSK::ECS {

	/// <summary>
	/// Componente para el renderizado 3D de un objeto.
	/// 
	/// Incluye:
	/// - Modelo 3D.
	/// - Material del modelo.
	/// - Instancias de los materiales de los meshes del modelo.
	/// </summary>
	class OSKAPI_CALL ModelComponent3D {

	public:

		OSK_COMPONENT("OSK::ModelComponent3D");

		/// <summary>
		/// Establece el modelo 3D que se renderizará.
		/// </summary>
		/// <param name="model">Modelo 3D cargado.</param>
		/// 
		/// @warning Debe ser establecido para poder ser usado.
		void SetModel(ASSETS::Model3D* model);

		/// <summary>
		/// Establece el material del modelo 3D.
		/// 
		/// @note Los meshes tendrán instancias de este material.
		/// </summary>
		/// 
		/// @warning Debe ser establecido para poder ser usado.
		void SetMaterial(GRAPHICS::Material* material);

		ASSETS::Model3D* GetModel() const;
		GRAPHICS::Material* GetMaterial() const;

		/// <summary>
		/// Devuelve la instancia del material del mesh dado.
		/// </summary>
		/// <param name="meshId">Índice del mesh del modelo.</param>
		/// 
		/// @warning No comprueba que esté dentro de los límites.
		GRAPHICS::MaterialInstance* GetMeshMaterialInstance(TSize meshId) const;

		/// <summary>
		/// Bindea la textura a todos los meshes.
		/// </summary>
		/// <param name="slot">Slot de la textura.</param>
		/// <param name="binding">Binding de la textura.</param>
		/// <param name="texture">Textura.</param>
		/// 
		/// @note Se guardará información, de tal manera que si se añaden más meshes
		/// después de haber llamado a la función, se bindearán a ellos también.
		/// 
		/// @warning No comprueba que el slot / textura existan en el material.
		void BindTextureForAllMeshes(const std::string& slot, const std::string& binding, const ASSETS::Texture* texture);

		/// <summary>
		/// Bindea la textura a todos los meshes.
		/// </summary>
		/// <param name="slot">Slot de la textura.</param>
		/// <param name="binding">Binding de la textura.</param>
		/// <param name="texture">Textura.</param>
		/// 
		/// @note Se guardará información, de tal manera que si se añaden más meshes
		/// después de haber llamado a la función, se bindearán a ellos también.
		/// 
		/// @warning No comprueba que el slot / textura existan en el material.
		void BindGpuImageForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::GpuImage* image);

		/// <summary>
		/// Bindea el buffer a todos los meshes.
		/// </summary>
		/// <param name="slot">Slot del buffer.</param>
		/// <param name="binding">Binding del buffer.</param>
		/// <param name="texture">Textura.</param>
		/// 
		/// @note Se guardará información, de tal manera que si se añaden más meshes
		/// después de haber llamado a la función, se bindearán a ellos también.
		/// 
		/// @warning No comprueba que el slot / buffer existan en el material.
		void BindUniformBufferForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::IGpuUniformBuffer* buffer);

	private:

		ASSETS::Model3D* model = nullptr;
		GRAPHICS::Material* material = nullptr;

		DynamicArray<OwnedPtr<GRAPHICS::MaterialInstance>> meshesMaterialInstances;

		HashMap<Pair<std::string, std::string>, const ASSETS::Texture*> texturesBound;
		HashMap<Pair<std::string, std::string>, const GRAPHICS::GpuImage*> imagesBound;
		HashMap<Pair<std::string, std::string>, const GRAPHICS::IGpuUniformBuffer*> uniformBuffersBound;

	};

}
