#pragma once

#include "IRenderer.h"
#include "UniquePtr.hpp"

#include <vulkan/vulkan.h>

namespace OSK::GRAPHICS {

	class IRenderpass;

	/// <summary>
	/// Implementación de la interfaz para el renderizador de Vulkan.
	/// </summary>
	class OSKAPI_CALL RendererVulkan : public IRenderer {

	public:

		RendererVulkan(bool requestRayTracing);
		~RendererVulkan();

		void Initialize(const std::string& appName, const Version& version, const IO::Window& window, PresentMode mode) override;
		void Close() override;
		void HandleResize() override;
		void PresentFrame() override;
		void SubmitSingleUseCommandList(OwnedPtr<ICommandList> commandList) override;

		OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) override;
		OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout, const VertexInfo& vertexInfo) override;
		OwnedPtr<IComputePipeline> _CreateComputePipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout& layout) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout& layout) const override;

		TSize GetCurrentFrameIndex() const override;
		TSize GetCurrentCommandListIndex() const override;

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
		static PFN_vkCmdEndDebugUtilsLabelEXT pvkCmdEndDebugUtilsLabelEXT;

		// Sin renderpasses
		static PFN_vkCmdBeginRendering pvkCmdBeginRendering;
		static PFN_vkCmdEndRendering pvkCmdEndRendering;

	protected:

		void CreateCommandQueues() override;
		void CreateSwapchain(PresentMode mode) override;
		void CreateSyncDevice() override;
		void CreateGpuMemoryAllocator() override;

	private:

		void CreateInstance(const std::string& appName, const Version& version);
		void SetupDebugLogging();
		void CreateSurface(const IO::Window& window);
		void ChooseGpu();

		// Sync
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

		VkInstance instance;
		VkSurfaceKHR surface;
		VkDebugUtilsMessengerEXT debugConsole;

		// Sync
		DynamicArray<VkSemaphore> imageAvailableSemaphores;

		DynamicArray<VkSemaphore> preComputeFinishedSemaphores; // PreCompute -> RenderFinished
		DynamicArray<VkSemaphore> renderFinishedSemaphores; // RenderFinished -> PostCompute
		DynamicArray<VkSemaphore> postComputeFinishedSemaphores; // PostCompute -> Framebuild
		DynamicArray<VkSemaphore> frameBuildSemaphores; // Framebuild -> Present

		DynamicArray<VkFence> fullyRenderedFences;

		TSize currentCommandBufferIndex = 0;
		TSize currentFrameIndex = 0;

		TSize vulkanVersion = VK_API_VERSION_1_3;

	};

}
