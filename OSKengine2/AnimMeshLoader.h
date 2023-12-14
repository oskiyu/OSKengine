#pragma once

#include "IGltfLoader.h"
#include "Vertex3D.h"
#include "Bone.h"
#include "Animator.h"


namespace OSK::ASSETS {

	/// <summary>
	/// Clase auxiliar que permite la carga de modelos 3D animados
	/// junto a sus animaciones.
	/// </summary>
	class AnimMeshLoader : public IGltfLoader {

	public:

		/// <summary>
		/// Configura el modelo 3D.
		/// También establece su animator.
		/// </summary>
		/// 
		/// @pre model no puede ser null.
		void SetupModel(Model3D* model) override;

	protected:

		void ProcessNode(const tinygltf::Model& model, const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) override;

		DynamicArray<GRAPHICS::VertexAnim3D> vertices;
		
		/// <summary>
		/// Este animator será transpasado al modelo 3D al llamar
		/// a AnimMeshLoader::SetupModel.
		/// </summary>
		GRAPHICS::Animator tempAnimator{};

	private:

		void LoadAnimations(const tinygltf::Model& model);
		void LoadSkins(const tinygltf::Model& model);

	};

}
