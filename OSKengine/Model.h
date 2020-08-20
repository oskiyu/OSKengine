#pragma once

#include "VulkanBuffer.h"
#include "Transform.h"
#include "PushConst3D.h"
#include "GraphicsPipeline.h"
#include "Vertex.h"
#include "Texture.h"
#include <vector>

namespace OSK {

	//V�rtices e �ndices de un modelo.
	struct TempModelData {
		//V�rtices.
		std::vector<Vertex> Vertices;
		//�ndices.
		std::vector<vertexIndex_t> Indices;
	};


	//Informaci�n de un modelo.
	//Contiene los buffers de los v�rtices y los �ndices.
	//Se almacena en la clase VulkanRenderer.
	struct ModelData {

	public:

		void Bind(VkCommandBuffer commandBuffer) const;

		VulkanBuffer VertexBuffer;

		VulkanBuffer IndexBuffer;

		//N�mero de �ndices.
		size_t IndicesCount = 0;
	};


	//Representa un modelo 3D.
	class Model {

	public:

		void Bind(VkCommandBuffer commandBuffer) const;

		void PushConstants(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline);

		void Draw(VkCommandBuffer commandBuffer) const;

		//Buffers del modelo.
		ModelData* Data;

		//Transform3D del modelo.
		Transform* ModelTransform;

		ModelTexture* texture = nullptr;

		//Obtiene el Push Constant con la matriz del modelo.
		PushConst3D GetPushConst() const;

		PushConst3D PushConst{};
	};

}