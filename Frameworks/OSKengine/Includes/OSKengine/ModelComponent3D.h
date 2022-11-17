#pragma once

#include "Component.h"
#include "OwnedPtr.h"
#include "UniquePtr.hpp"
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

		/// <summary> Establece el modelo 3D que se renderizar�. </summary>
		/// <param name="model">Modelo 3D cargado.</param>
		/// 
		/// @pre model no debe ser null.
		/// @warning Debe ser establecido para poder ser usado.
		void SetModel(ASSETS::Model3D* model);

		/// <summary> Establece los material instances del modelo 3D. </summary>
		/// 
		/// @note Los meshes tendr�n instancias de este material.
		/// @warning Debe ser establecido para poder ser usado.
		void SetMaterial(GRAPHICS::Material* material);


		/// <summary> Devuelve el modelo 3D del componente. </summary>
		/// <returns>No null.</returns>
		/// 
		/// @pre Debe haberse establecido el modelo con ModelComponent3D::SetModel.
		ASSETS::Model3D* GetModel() const;


		/// <summary> Devuelve la instancia del material del mesh dado. </summary>
		/// <param name="meshId">�ndice del mesh del modelo.</param>
		/// 
		/// @pre El id debe corresponder a un mesh existente dentro del modelo 3D.
		/// @warning No comprueba que est� dentro de los l�mites.
		GRAPHICS::MaterialInstance* GetMeshMaterialInstance(TSize meshId) const;

		/// <summary> Bindea la textura a todos los meshes. </summary>
		/// <param name="slot">Slot de la textura.</param>
		/// <param name="binding">Binding de la textura.</param>
		/// <param name="texture">Textura.</param>
		/// 
		/// @note Se guardar� informaci�n, de tal manera que si se a�aden m�s meshes
		/// despu�s de haber llamado a la funci�n, se bindear�n a ellos tambi�n.
		/// 
		/// @warning No comprueba que el slot / textura existan en el material.
		void BindTextureForAllMeshes(const std::string& slot, const std::string& binding, const ASSETS::Texture* texture);

		/// <summary> Bindea la textura a todos los meshes. </summary>
		/// <param name="slot">Slot de la textura.</param>
		/// <param name="binding">Binding de la textura.</param>
		/// <param name="texture">Textura.</param>
		/// 
		/// @note Se guardar� informaci�n, de tal manera que si se a�aden m�s meshes
		/// despu�s de haber llamado a la funci�n, se bindear�n a ellos tambi�n.
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
		/// @note Se guardar� informaci�n, de tal manera que si se a�aden m�s meshes
		/// despu�s de haber llamado a la funci�n, se bindear�n a ellos tambi�n.
		/// 
		/// @warning No comprueba que el slot / buffer existan en el material.
		void BindUniformBufferForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::IGpuUniformBuffer* buffer);

	private:

		ASSETS::Model3D* model = nullptr;
		GRAPHICS::Material* material = nullptr;

		DynamicArray<UniquePtr<GRAPHICS::MaterialInstance>> meshesMaterialInstances;

		/// <summary> Texturas enlazadas para todos los meshes. </summary>
		HashMap<Pair<std::string, std::string>, const ASSETS::Texture*> texturesBound;
		/// <summary> Texturas enlazadas para todos los meshes. </summary>
		HashMap<Pair<std::string, std::string>, const GRAPHICS::GpuImage*> imagesBound;
		/// <summary> Buffers enlazados para todos los meshes. </summary>
		HashMap<Pair<std::string, std::string>, const GRAPHICS::IGpuUniformBuffer*> uniformBuffersBound;

	};

}
