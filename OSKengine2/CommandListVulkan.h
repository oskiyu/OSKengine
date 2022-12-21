#pragma once

#include "ICommandList.h"
#include "DynamicArray.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class GpuDataBuffer;
	class GraphicsPipelineVulkan;

	/// <summary>
	/// Una lista de comandos contiene una serie de comandos que ser�n
	/// enviados a la GPU para su ejecuci�n.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecuci�n.
	/// 
	/// Esta es la implementaci�n de la lista de comandos para el 
	/// renderizador de Vulkan.
	/// 
	/// El n�mero de listas de comandos nativas depender� del n�mero de
	/// im�genes en el swapchain.
	/// Esta clase representa una abstracci�n de una lista de comandos:
	/// cada comando grabado en la clase se grabar� en cada una de las
	/// listas nativas.
	/// </summary>
	class OSKAPI_CALL CommandListVulkan : public ICommandList {

	public:

		const DynamicArray<VkCommandBuffer>& GetCommandBuffers() const;
		DynamicArray<VkCommandBuffer>* GetCommandBuffers();

		void Reset() override;
		void Start() override;
		void Close() override;

		void BeginGraphicsRenderpass(DynamicArray<RenderPassImageInfo> colorImages, RenderPassImageInfo depthImage, const Color& color);
		void EndGraphicsRenderpass() override;

		void BindMaterial(Material* material) override;
		void BindVertexBuffer(const IGpuVertexBuffer* buffer) override;
		void BindIndexBuffer(const IGpuIndexBuffer* buffer) override;
		void BindMaterialSlot(const IMaterialSlot* slot) override;
		void PushMaterialConstants(const std::string& pushConstName, const void* data, TSize size, TSize offset) override;

		void DrawSingleInstance(TSize numIndices) override;
		void DrawSingleMesh(TSize firstIndex, TSize numIndices) override;
		void TraceRays(TSize raygenEntry, TSize closestHitEntry, TSize missEntry, const Vector2ui& resolution) override;

		void BindComputePipeline(const IComputePipeline& computePipeline);
		void DispatchCompute(const Vector3ui& groupCount);

		void SetGpuImageBarrier(GpuImage* image, GpuImageLayout previousLayout, GpuImageLayout nextLayout, GpuBarrierInfo previous, GpuBarrierInfo next, const GpuImageBarrierInfo& prevImageInfo) override;

		void CopyBufferToImage(const GpuDataBuffer* source, GpuImage* dest, TSize layer, TSize offset) override;
		void CopyImageToImage(const GpuImage* source, GpuImage* destination, const CopyImageInfo& copyInfo) override;

		/// <summary>
		/// Copia los contenidos del primer buffer al segundo.
		/// </summary>
		/// <param name="source">Buffer donde est�n el contenido que se quiere copiar.</param>
		/// <param name="dest">Buffer en el que se va a poner el contenido copiado.</param>
		/// <param name="size">Tama�o, en bytes, del �rea de memoria que se va a copiar.</param>
		/// <param name="sourceOffset">Offset del �rea de memoria en el origen.</param>
		/// <param name="destOffset">Offset del �rea de memoria en el destino.</param>
		void CopyBuffer(const GpuDataBuffer* source, GpuDataBuffer* dest, TSize size, TSize sourceOffset, TSize destOffset);

		void SetViewport(const Viewport& viewport) override;
		void SetScissor(const Vector4ui& scissor) override;

		void SetDebugName(const std::string& name) override;

	private:

		VkPipelineStageFlagBits GetPipelineStage(GpuBarrierStage stage) const;
		VkAccessFlags GetPipelineAccess(GpuBarrierAccessStage stage) const;

		/// <summary>
		/// Varias listas nativas, una por cada imagen en el swapchain.
		/// </summary>
		DynamicArray<VkCommandBuffer> commandBuffers;

	};

}
