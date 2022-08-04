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

		RendererVulkan();
		~RendererVulkan();

		void Initialize(const std::string& appName, const Version& version, const IO::Window& window, PresentMode mode) override;
		void Close() override;
		void HandleResize() override;
		void PresentFrame() override;
		void SubmitSingleUseCommandList(ICommandList* commandList) override;

		OwnedPtr<IGraphicsPipeline> _CreateGraphicsPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const VertexInfo& vertexInfo) override;
		OwnedPtr<IRaytracingPipeline> _CreateRaytracingPipeline(const PipelineCreateInfo& pipelineInfo, const MaterialLayout* layout, const VertexInfo& vertexInfo) override;
		OwnedPtr<IMaterialSlot> _CreateMaterialSlot(const std::string& name, const MaterialLayout* layout) const override;

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

		void SetupRtFunctions(VkDevice logicalDevice);

		bool AreValidationLayersAvailable() const;

		VkInstance instance;
		VkSurfaceKHR surface;
		VkDebugUtilsMessengerEXT debugConsole;

	};

}
