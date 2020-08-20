#pragma once

#include "VulkanBuffer.h"
#include "Transform.h"
#include "PushConst3D.h"
#include "GraphicsPipeline.h"
#include "Vertex.h"
#include "Texture.h"
#include <vector>

namespace OSK {

	//Vértices e índices de un modelo.
	struct TempModelData {
		//Vértices.
		std::vector<Vertex> Vertices;
		//Índices.
		std::vector<vertexIndex_t> Indices;
	};


	//Información de un modelo.
	//Contiene los buffers de los vértices y los índices.
	//Se almacena en la clase VulkanRenderer.
	struct ModelData {

	public:

		void Bind(VkCommandBuffer commandBuffer) const;

		VulkanBuffer VertexBuffer;

		VulkanBuffer IndexBuffer;

		//Número de índices.
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