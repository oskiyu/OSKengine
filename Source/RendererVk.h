#pragma once

#include "IRenderer.h"
#include "UniquePtr.hpp"

#include <vulkan/vulkan.h>
#include "ResourcesInFlight.h"

namespace OSK::GRAPHICS {

	class IRenderpass;

	class OSKAPI_CALL RendererVk final : public IRenderer {

	public:

		explicit RendererVk(bool requestRayTracing);
		~RendererVk();

		void Initialize(
			const std::string& appName, 
			const Version& version, 
			const IO::IDisplay& display, 
			PresentMode mode) override;
		void Close() override;
		void HandleResize(const Vector2ui& resolution) override;
		void PresentFrame() override;
		void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) override;
		void WaitForCompletion() override;

		OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout, 
			const VertexInfo& vertexInfo) override;
		OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout, 
			const VertexInfo& vertexInfo) override;
		OwnedPtr<IComputePipeline> _CreateComputePipeline(
			const PipelineCreateInfo& pipelineInfo, 
			const MaterialLayout& layout) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(
			const std::string& name, 
			const MaterialLayout& layout) const override;

		OwnedPtr<ICommandPool> CreateCommandPool(const ICommandQueue* targetQueueType) override;

		UIndex32 GetCurrentFrameIndex() const override;
		UIndex32 GetCurrentCommandListIndex() const override;

		bool SupportsRaytracing() const override;

		// Funciones que deben ser cargadas por ser de extensiones.
		static PFN_vkGetBufferDeviceAddressKHR pvkGetBufferDeviceAddressKHR;
		static PFN_vkCmdBuildAccelerationStructuresKHR pvkCmdBuildAccelerationStructuresKHR;
		static PFN_vkBuildAccelerationStructuresKHR pvkBuildAccelerationStructuresKHR;
		static PFN_vkCreateAccelerationStructureKHR pvkCreateAccelerationStructureKHR;
		static PFN_vkDestroyAccelerationStructureKHR pvkDestroyAccelerationStructureKHR;
		static PFN_vkGetAccelerationStructureBuildSizesKHR pvkGetAccelerationStructureBuildSizesKHR;
		static PFN_vkGetAccelerationStructureDeviceAddressKHR pvkGetAccelerationStructureDeviceAddressKHR;
		static PFN_vkCmdTraceRaysKHR pvkCmdTraceRaysKHR;
		static PFN_vkGetRayTracingShaderGroupHandlesKHR pvkGetRayTracingShaderGroupHandlesKHR;
		static PFN_vkCreateRayTracingPipelinesKHR pvkCreateRayTracingPipelinesKHR;

		// Debug markers
		static PFN_vkSetDebugUtilsObjectNameEXT pvkSetDebugUtilsObjectNameEXT;
		static PFN_vkSetDebugUtilsObjectTagEXT pvkSetDebugUtilsObjectTagEXT;
		static PFN_vkCmdDebugMarkerBeginEXT pvkCmdDebugMarkerBeginEXT;
		static PFN_vkCmdInsertDebugUtilsLabelEXT pvkCmdInsertDebugUtilsLabelEXT;
		static PFN_vkCmdBeginDebugUtilsLabelEXT pvkCmdBeginDebugUtilsLabelEXT;
		static PFN_vkCmdEndDebugUtilsLabelEXT pvkCmdEndDebugUtilsLabelEXT;

		// Sin renderpasses
		static PFN_vkCmdBeginRendering pvkCmdBeginRendering;
		static PFN_vkCmdEndRendering pvkCmdEndRendering;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain(PresentMode mode, const Vector2ui& resolution) override;
		void CreateGpuMemoryAllocator() override;

	private:

		void CreateInstance(const std::string& appName, const Version& version);
		void SetupDebugLogging();
		void CreateSurface(const IO::IDisplay& display);
		/// @throws GpuNotFoundException Si no se encuentra ninguna GPU compatible.
		/// @throws LogicalDeviceCreationException Si ocurre algún error al establecer la conexión lógica.
		void ChooseGpu();

		// Sync
		void CreateSyncPrimitives();
		void SubmitMainCommandList();

		void SubmitFrame();
		void AcquireNextFrame();

		void SetupRtFunctions(VkDevice logicalDevice);
		void SetupDebugFunctions(VkDevice instance);
		void SetupRenderingFunctions(VkDevice logicalDevice);

		bool AreValidationLayersAvailable() const;

		VkInstance m_instance = VK_NULL_HANDLE;
		VkSurfaceKHR m_surface = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_debugConsole = VK_NULL_HANDLE;

		// Sync

		/// @brief Semáforos que serán señalizados
		/// cuando la imagen indicada esté disponible
		/// para renderizarse.
		std::array<VkSemaphore, MAX_RESOURCES_IN_FLIGHT> m_imageAvailableSemaphores;

		/// @brief Semáforos que serán señalizados
		/// cuando la imagen indicada haya terminado de
		/// renderizarse.
		std::array<VkSemaphore, MAX_RESOURCES_IN_FLIGHT> m_imageFinishedSemaphores;

		/// @brief Permiten esperar en la CPU a que la siguiente
		/// imagen haya terminado de renderizarse.
		DynamicArray<VkFence> m_fullyRenderedFences;

		USize32 currentCommandBufferIndex = 0;
		USize32 currentFrameIndex = 0;

		USize32 vulkanVersion = VK_API_VERSION_1_3;

	};

}
