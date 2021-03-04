#pragma once

#include "VulkanBuffer.h"
#include "Transform.h"
#include "PushConst3D.h"
#include "GraphicsPipeline.h"
#include "Vertex.h"
#include "Texture.h"
#include <vector>

namespace OSK {

	struct OSKAPI_CALL AnimUBO {

		AnimUBO() {
			for (auto& i : Bones) {
				i = glm::mat4(1.0f);
			}
		}

		std::array<glm::mat4, OSK_ANIM_MAX_BONES> Bones;
	};
		
	//Vértices e índices de un modelo.
	struct OSKAPI_CALL TempModelData {
		//Vértices.
		std::vector<Vertex> Vertices;
		//Índices.
		std::vector<vertexIndex_t> Indices;
	};


	//Información de un modelo.
	//Contiene los buffers de los vértices y los índices.
	//Se almacena en la clase VulkanRenderer.
	struct OSKAPI_CALL ModelData {

	public:

		//Enlaza los vértices y los índices del modelo.
		inline void Bind(VkCommandBuffer commandBuffer) const {
			const VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &VertexBuffer.Buffer, offsets);
			vkCmdBindIndexBuffer(commandBuffer, IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32);
		}

		//Renderiza el modelo, individualmente.
		inline void Draw(VkCommandBuffer commandBuffer) const {
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(IndicesCount), 1, 0, 0, 0);
		}

		//Vértices del modelo.
		VulkanBuffer VertexBuffer;

		//Índices del modelo.
		VulkanBuffer IndexBuffer;

		//Número de índices.
		size_t IndicesCount = 0;
	};


	//Representa un modelo 3D.
	class OSKAPI_CALL Model {

	public:

		//Enlaza los vértices y los índices del modelo.
		inline void Bind(VkCommandBuffer commandBuffer) const {
			Data->Bind(commandBuffer);
		}

		//Envía las constantes 3D a la GPU.
		inline void PushConstants(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline) {
			PushConst = GetPushConst();
			vkCmdPushConstants(commandBuffer, pipeline->VulkanPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConst3D), &PushConst);
		}

		//Renderiza el modelo, individualmente.
		inline void Draw(VkCommandBuffer commandBuffer) const {
			Data->Draw(commandBuffer);
		}

		//Buffers del modelo.
		ModelData* Data;

		//Transform3D del modelo.
		Transform ModelTransform;

		//Textura del modelo.
		ModelTexture* texture = nullptr;

		//Obtiene el Push Constant con la matriz del modelo.
		inline PushConst3D GetPushConst() {
			ModelTransform.UpdateModel();

			PushConst3D pushConst{};
			pushConst.model = ModelTransform.ModelMatrix;

			return pushConst;
		}

		inline void UpdateAnimUBO(std::vector<VulkanBuffer>& buffers) {
			for (auto& i : buffers) {
				i.Write(BonesUBOdata.Bones.data(), sizeof(glm::mat4) * BonesUBOdata.Bones.size(), i.Alignment * AnimationBufferOffset);
			}
		}

		uint32_t AnimationBufferOffset = 0;

		AnimUBO BonesUBOdata;
		VkDevice LogicalDevice = VK_NULL_HANDLE;

		//Constantes 3D.
		PushConst3D PushConst{};
		uint32_t NumberOfBones = 0;

	};

}