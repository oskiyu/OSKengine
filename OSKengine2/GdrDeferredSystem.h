#pragma once

#include "RenderSystem3D.h"

#include "DeferredRenderSystem.h"

#include "GpuBuffer.h"

namespace OSK::ECS {

	class CameraComponent3D;


	class OSKAPI_CALL GdrDeferredRenderSystem : public DeferredRenderSystem {

	public:

		OSK_SYSTEM("OSK::GdrDeferredRenderSystem");

		GdrDeferredRenderSystem();

		void SetMaxCounts(USize32 maxVertexCount, USize32 maxMeshCount);

		virtual void Render(GRAPHICS::ICommandList* commandList) override;

	private:

		void CreateUnifiedBuffers(USize32 maxVertexCount, USize32 maxMeshCount);

		struct GdrPerMeshInfo {
			UIndex32 positionsOffset = 0;
			UIndex32 attributesOffset = 0;
			UIndex32 animationAttributesOffset = 0;

			UIndex32 materialOffset = 0;

			glm::mat4 modelMatrix = glm::mat4(1.0f);
			glm::mat4 previousModelMatrix = glm::mat4(1.0f);
		};

		struct GdrPerMaterialInfo {
			Vector2f metallicRoughness = Vector2f::Zero;
			Color emissiveColor = Color::White * 0.0f;

			UIndex32 albedoTextureIndex = 0;
			UIndex32 normalTextureIndex = 0;
		};

		struct MeshDrawInfo {
			/// @brief Posición del primer vértice en el buffer unificado.
			UIndex32 firstVertexIndex = 0;
		};

	private:

		USize32 m_maxVertices = 0;


		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_unifiedVertexBuffers{};
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_unifiedIndexBuffers{};


		// -- Vertex attributes -- //

		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_vertexPositionsBuffers{};
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_vertexAttributesBuffers{};
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_vertexAnimAttributesBuffers{};


		// Textures & descriptor sets

		GRAPHICS::Material* m_gdrMaterial;
		/// @todo Crear
		UniquePtr<GRAPHICS::MaterialInstance> m_gdrMaterialInstance{};


		// -- Tables -- //

		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_perMeshTable{};
		std::array<UniquePtr<GRAPHICS::GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> m_perMaterialTable{};

		
		// -- Previous frames -- //

		std::unordered_map<GameObjectIndex, glm::mat4> m_previousFrameMatrices{};

	};

}
