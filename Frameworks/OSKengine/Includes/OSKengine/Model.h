#pragma once

#include "GPUDataBuffer.h"
#include "Transform.h"
#include "PushConst3D.h"
#include "GraphicsPipeline.h"
#include "Vertex.h"
#include "Texture.h"
#include <vector>

#include "OldMaterialInstance.h"
#include "AnimUBO.h"
#include "ModelData.h"

namespace OSK {
	
	/// <summary>
	/// Representa un modelo 3D.
	/// </summary>
	class OSKAPI_CALL Model {

		friend class ContentManager;
		friend class RenderizableScene;
		friend class RenderSystem3D;

	public:

		/// <summary>
		/// Material del modelo.
		/// </summary>
		SharedPtr<OldMaterialInstance> material;

		/// <summary>
		/// Material para el renderizado de sombras.
		/// </summary>
		SharedPtr<OldMaterialInstance> shadowMaterial;

		Transform* GetTransform() {
			return &transform;
		}

	protected:

		/// <summary>
		/// UBO de la animación.
		/// </summary>
		AnimUBO bonesUBOdata;

	private:

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
		ModelData* data = nullptr;

		/// <summary>
		/// Transform3D del modelo.
		/// </summary>
		Transform transform;

		/// <summary>
		/// Obtiene el Push Constant con la matriz del modelo.
		/// </summary>
		/// <returns>Push constant.</returns>
		PushConst3D GetPushConst();

		/// <summary>
		/// Actualiza el UBO de animación.
		/// </summary>
		/// <param name="buffers">Buffers del UBO.</param>
		void UpdateAnimUBO(std::vector<SharedPtr<GpuDataBuffer>>& buffers);

		/// <summary>
		/// Offset de la animación de este modelo respecto al buffer dinámico de la animación.
		/// </summary>
		uint32_t animationBufferOffset = 0;

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice logicalDevice = VK_NULL_HANDLE;

		/// <summary>
		/// Constantes 3D.
		/// </summary>
		PushConst3D pushConst{};

		/// <summary>
		/// Número de huesos.
		/// </summary>
		uint32_t numberOfBones = 0;

	};

}