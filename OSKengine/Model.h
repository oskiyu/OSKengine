#pragma once

#include "GPUDataBuffer.h"
#include "Transform.h"
#include "PushConst3D.h"
#include "GraphicsPipeline.h"
#include "Vertex.h"
#include "Texture.h"
#include <vector>

#include "MaterialInstance.h"
#include "AnimUBO.h"
#include "ModelData.h"

namespace OSK {
	
	/// <summary>
	/// Representa un modelo 3D.
	/// </summary>
	class OSKAPI_CALL Model {

	public:

		/// <summary>
		/// Enlaza los vértices y los índices del modelo para su renderizado.
		/// </summary>
		/// <param name="commandBuffer">Commandbuffer.</param>
		void Bind(VkCommandBuffer commandBuffer) const;

		/// <summary>
		/// Envía las constantes 3D de este modelo a la GPU.
		/// </summary>
		/// <param name="commandBuffer">Commandbuffer.</param>
		/// <param name="pipeline">Pipeline activa.</param>
		void PushConstants(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline);

		/// <summary>
		/// Renderiza el modelo, individualmente.
		/// Debe llamarse antes a Bind y a PushConstants.
		/// </summary>
		/// <param name="commandBuffer">Command buffer.</param>
		void Draw(VkCommandBuffer commandBuffer) const;

		/// <summary>
		/// Buffers del modelo.
		/// </summary>
		ModelData* Data;

		/// <summary>
		/// Transform3D del modelo.
		/// </summary>
		Transform ModelTransform;

		/// <summary>
		/// Material del modelo.
		/// </summary>
		SharedPtr<MaterialInstance> ModelMaterial;

		/// <summary>
		/// Material para el renderizado de sombras.
		/// </summary>
		SharedPtr<MaterialInstance> ShadowMaterial;

		//Obtiene el Push Constant con la matriz del modelo.

		/// <summary>
		/// Obtiene el Push Constant con la matriz del modelo.
		/// </summary>
		/// <returns>Push constant.</returns>
		PushConst3D GetPushConst();

		/// <summary>
		/// Actualiza el UBO de animación.
		/// </summary>
		/// <param name="buffers">Buffers del UBO.</param>
		void UpdateAnimUBO(std::vector<GPUDataBuffer>& buffers);

		/// <summary>
		/// Offset de la animación de este modelo respecto al buffer dinámico de la animación.
		/// </summary>
		uint32_t AnimationBufferOffset = 0;

		/// <summary>
		/// UBO de la animación.
		/// </summary>
		AnimUBO BonesUBOdata;

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice LogicalDevice = VK_NULL_HANDLE;

		/// <summary>
		/// Constantes 3D.
		/// </summary>
		PushConst3D PushConst{};

		/// <summary>
		/// Número de huesos.
		/// </summary>
		uint32_t NumberOfBones = 0;

	};

}