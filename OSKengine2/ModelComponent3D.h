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
	/// Incluye:
	/// - Modelo 3D.
	/// - Material del modelo.
	/// - Instancias de los materiales de los meshes del modelo.
	/// </summary>
	class OSKAPI_CALL ModelComponent3D {

	public:

		OSK_COMPONENT("OSK::ModelComponent3D");

		~ModelComponent3D();

		void SetModel(ASSETS::Model3D* model);
		void SetMaterial(GRAPHICS::Material* materialInstance);

		ASSETS::Model3D* GetModel() const;
		GRAPHICS::Material* GetMaterial() const;

		GRAPHICS::MaterialInstance* GetMeshMaterialInstance(TSize meshId) const;

		void BindTextureForAllMeshes(const std::string& slot, const std::string& binding, const ASSETS::Texture* texture);
		void BindGpuImageForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::GpuImage* image);
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
