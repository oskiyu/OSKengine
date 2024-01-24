#pragma once

#include "IRenderer.h"
#include "UniquePtr.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class IRenderpass;

	/// <summary>
	/// Implementación de la interfaz para el renderizador de Vulkan.
	/// </summary>
	class OSKAPI_CALL RendererVk final : public IRenderer {

	public:

		RendererVk(bool requestRayTracing);
		~RendererVk();

		void Initialize(
			const std::string& appName, 
			const Version& version, 
			const IO::IDisplay& display, 
			PresentMode mode) override;
		void Close() override;
		void HandleResize() override;
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
		void CreateSwapchain(PresentMode mode) override;
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
		void SubmitPreComputeCommands();
		void SubmitGraphicsCommands();
		void SubmitPostComputeCommands();

		void SubmitGraphicsAndComputeCommands();
		void SubmitFrameBuildCommands();

		void SubmitFrame();
		void AcquireNextFrame();

		void SetupRtFunctions(VkDevice logicalDevice);
		void SetupDebugFunctions(VkDevice instance);
		void SetupRenderingFunctions(VkDevice logicalDevice);

		bool AreValidationLayersAvailable() const;

		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugConsole = VK_NULL_HANDLE;

		// Sync
		DynamicArray<VkSemaphore> imageAvailableSemaphores;

		DynamicArray<VkSemaphore> preComputeFinishedSemaphores; // PreCompute -> RenderFinished
		DynamicArray<VkSemaphore> renderFinishedSemaphores; // RenderFinished -> PostCompute
		DynamicArray<VkSemaphore> postComputeFinishedSemaphores; // PostCompute -> Framebuild
		DynamicArray<VkSemaphore> frameBuildSemaphores; // Framebuild -> Present

		DynamicArray<VkFence> fullyRenderedFences;

		USize32 currentCommandBufferIndex = 0;
		USize32 currentFrameIndex = 0;

		USize32 vulkanVersion = VK_API_VERSION_1_3;

	};

}
