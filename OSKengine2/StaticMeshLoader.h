#pragma once

#include "IGltfLoader.h"
#include "Vertex3D.h"

namespace OSK::ASSETS {

	/// <summary>
	/// Clase auxiliar que permite la carga de modelos 3D estáticos.
	/// </summary>
	class StaticMeshLoader : public IGltfLoader {

	public:

		/// <summary> Configura el modelo 3D. </summary>
		/// 
		/// @pre model no puede ser null.
		void SetupModel(Model3D* model) override;

	protected:

		void ProcessNode(const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) override;

		DynamicArray<GRAPHICS::Vertex3D> vertices;

	};

}
