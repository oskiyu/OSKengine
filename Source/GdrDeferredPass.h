#pragma once

#include "ApiCall.h"
#include "RenderPass.h"
#include "RenderGraphArgs.h"

#include <span>
#include "GlobalIndexBuffer.h"
#include "DynamicArray.hpp"
#include "EntityComponentSystem.h"

namespace OSK::GRAPHICS {

	class RenderGraph;
	struct VertexAttributeEntry;

	struct GdrDrawCall_Model {
		glm::mat4 matrix;
		glm::mat4 prviousMatrix;
	};

	struct GdrDrawCall_MaterialInfo {
		Vector2f metallicRoughness = Vector2f::Zero;
		Color emissive = Color::Empty;

		UIndex32 albedoImageIdx = 0;
		UIndex32 normalImageIdx = 0;
	};

	struct VertexAttribBufferRef {
		RgBufferRef bufferRef;
		std::string bindingName = "";
	};
	
	DynamicArray<VertexAttribBufferRef> VertexAttribBufferRefToList(std::span<const VertexAttributeEntry> entries, const RenderGraph& rg);


	class OSKAPI_CALL GdrDeferredPass : public IRenderPass {

	public:

		explicit GdrDeferredPass(
			RenderGraph* rg,
			std::span<const GdrBufferUuid> attributes,
			GdrBufferUuid indexes,
			GdrBufferUuid attribsIndexes,
			std::span<const GdrImageUuid> colorTargets,
			GdrImageUuid depthTarget,
			std::span<const GdrBufferUuid> cameraBuffer,
			std::span<const GdrBufferUuid> previousCameraBuffer);

		void RegisterFullDependencies(
			std::span<const VertexAttribBufferRef> attributes,
			const RgBufferRef& indexes,
			const RgBufferRef& attribsIndexes,
			std::span<const RgBufferRef> cameraBuffer,
			std::span<const RgBufferRef> previousCameraBuffer);

		void Execute(ICommandList* cmdList) override;

		std::string_view GetName() const override {
			return "OSK::GdrDeferredPass";
		}

	public:


		/// @brief Estructura con la información almacenada
		/// en los uniform buffers de la cámara.
		struct CameraInfo {
			alignas(16) glm::mat4 projectionMatrix;
			alignas(16) glm::mat4 viewMatrix;
			alignas(16) glm::mat4 projectionViewMatrix;

			alignas(16) Vector4f position;

			alignas(16) Vector2f nearFarPlanes;
		};

		/// @brief Estructura con la información almacenada
		/// en los uniform buffers de la cámara del frame
		/// anterior.
		struct PreviousCameraInfo {
			alignas(16) glm::mat4 projectionMatrix;
			alignas(16) glm::mat4 viewMatrix;
			alignas(16) glm::mat4 projectionViewMatrix;
		};


	private:

		ECS::ExternalEcsListUuid m_objectListUuid = ECS::ExternalEcsListUuid::CreateEmpty();

		RenderGraph* m_renderGraph;

		GdrBufferUuid m_indexBuffer;
		GdrBufferUuid m_instancesBuffer;
		GdrBufferUuid m_materialsBuffer;

		std::unordered_map<GdrImageUuid, USize32> m_imageIndexes;

		DynamicArray<GdrDrawCall_MaterialInfo> m_drawCalls_Materials;
		DynamicArray<GdrDrawCall_Model>		   m_drawCalls_Models;
		DynamicArray<GlobalIndexBufferRange>   m_drawCallsIndexes;

	};

}
