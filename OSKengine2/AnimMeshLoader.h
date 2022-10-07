#pragma once

#include "IMeshLoader.h"
#include "Vertex3D.h"
#include "Bone.h"
#include "Animator.h"

namespace OSK::ASSETS {

	/// <summary>
	/// 
	/// </summary>
	/// 
	/// @todo Carga correcta con el tamaño indicado en el archivo JSON.
	class AnimMeshLoader : public IMeshLoader {

	public:

		void SetupModel(Model3D* model) override;

	protected:

		void ProcessNode(const tinygltf::Node& node, TSize nodeId, TSize parentId, const glm::mat4& prevMat) override;
		void SmoothNormals() override;

		DynamicArray<GRAPHICS::VertexAnim3D> vertices;
		
		GRAPHICS::Animator tempAnimator{};

	private:

		void LoadAnimations();
		void LoadSkins();

	};

}
