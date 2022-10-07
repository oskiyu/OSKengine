#pragma once

#include "IMeshLoader.h"
#include "Vertex3D.h"

namespace OSK::ASSETS {


	class StaticMeshLoader : public IMeshLoader {

	public:

		void SetupModel(Model3D* model) override;

	protected:

		void ProcessNode(const tinygltf::Node& node, TSize nodeId, TSize parentId, const glm::mat4& prevMat) override;
		void SmoothNormals() override;

		DynamicArray<GRAPHICS::Vertex3D> vertices;

	};

}
