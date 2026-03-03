#pragma once

#include "ApiCall.h"
#include "DynamicArray.hpp"
#include "UniquePtr.hpp"
#include "HashMap.hpp"

#include "GpuBuffer.h"
#include "GpuBufferRange.h"
#include "IGpuImage.h"
#include "GpuImageRange.h"

#include "RenderPass.h"
#include "RenderTarget.h"
#include "RenderPassDependencies.h"

#include "Vector3.hpp"
#include "Vector2.hpp"

#include "OSKengine.h"

#include "VertexAttributes.h"
#include <unordered_set>

#include "GlobalVertexBuffer.h"
#include "GlobalIndexBuffer.h"

#include "GpuGeometryDesc.h"

#include "RenderGraphArgs.h"

#include "ImageUuid.h"

#include "RgBufferManager.h"
#include "RgImageManager.h"

#include "RgEntry.h"

#include "RgGeometryManager.h"

namespace OSK::GRAPHICS {

	class ICommandList;
	class IRenderer;

	class GpuModel3D;
		

	/// @brief Grafo de renderizado,
	/// que contiene tanto los pases de renderizado
	/// como los recursos que estos necesitan.
	class OSKAPI_CALL RenderGraph {

	public:

		explicit RenderGraph(IRenderer* renderer);

		/// @brief Registra un nuevo buffer, que será
		/// creado y gestionado por el render-graph.
		/// @param info Información del buffer.
		/// 
		/// @return UUID del buffer.
		GdrBufferUuid RegisterBuffer(const RgBufferRegisterInfo& info);

		/// @brief Registra una nueva imagen, que
		/// será creada y gestionada por el render-graph.
		/// @param info Información de la imagen.
		/// @param size Resolución de la imagen, relativa a
		/// la resolución del framebuffer.
		/// 
		/// @return UUID de la imagen.
		GdrImageUuid RegisterImage(
			const RgImageRegisterInfo& info,
			RgRelativeSizeImageRegisterArgs size);

		/// @brief Registra una nueva imagen, que
		/// será creada y gestionada por el render-graph.
		/// @param info Información de la imagen.
		/// @param size Resolución de la imagen.
		/// 
		/// @return UUID de la imagen.
		GdrImageUuid RegisterImage(
			const RgImageRegisterInfo& info,
			RgAbsoluteSizeImageRegisterArgs size);

		GdrImageUuid RegisterExternalImage(GpuImage* image);

		/// @brief Registra un nuevo render-pass.
		/// @param pass Render-pass.
		/// @param passDependencies Dependencias del renderpass,
		/// respecto a otros renderpasses.
		/// 
		/// @note Si ya existe un renderpass con nombre (ID) @p name, 
		/// entonces se sobreescribirá.
		RenderPassUuid RegisterRenderpass(
			UniquePtr<IRenderPass>&& pass,
			const RenderPassDependencies& passDependencies);


		/// @brief Crea todos los recursos, y el render-graph
		/// pasa a estar en estado ejecutable.
		void Compile();


		/// @brief Ejecuta el render-graph.
		/// @param cmdList Lista de comados a usar.
		/// 
		/// @pre Debe haberse llamado previamente a `Compile()`.
		/// @pre La lista de comandos debe estar abierta.
		/// @pre La lista de comandos no debe estar dentro
		/// de un proceso de rasterizado.
		void Execute(ICommandList* cmdList);


		/// @brief Registra un nuevo tipo de atributo de
		/// vértice.
		/// @param uuid ID único del atributo.
		/// @param attributeSize Tamaño de cada atributo
		/// individual.
		/// 
		/// @note Si ya existe un atributo con nombre (ID) @p name, 
		/// entonces se sobreescribirá.
		void RegisterVertexAttribute(
			VertexAttribUuid uuid,
			std::string name,
			USize32 attributeSize);


		/// @param uuid UUID del buffer.
		/// @return Buffer indicado.
		/// @pre Debe existir un buffer con el 
		/// UUID indicado.
		GpuBuffer& GetBuffer(GdrBufferUuid uuid);
		const GpuBuffer& GetBuffer(GdrBufferUuid uuid) const;

		/// @param uuid UUID de la imagen.
		/// @return Imagen indicada.
		/// @pre Debe existir una imagen con el 
		/// UUID indicado.
		GpuImage& GetImage(GdrImageUuid uuid);
		const GpuImage& GetImage(GdrImageUuid uuid) const;

		GpuGeometryEntry RegisterGeometry(const GpuGeometryDesc& geometry);

		std::span<const VertexAttributeEntry> GetRegisteredAttributes() const;
		std::span<const GdrBufferUuid> GetRegisteredAttributesBufferUuids() const;
		RgBufferRef GetAttribsIndexes() const;
		RgBufferRef GetIndexesBuffer() const;
		void CreateGeometryBuffers();

		void SetFramebufferResolution(USize32   res);
		void SetFramebufferResolution(Vector2ui res);
		void SetFramebufferResolution(Vector3ui res);


		IRenderPass* GetRenderPass(RenderPassUuid uuid);
		const IRenderPass* GetRenderPass(RenderPassUuid uuid) const;

	private:

		void RegisterUnifiedGeometryBuffers();

		void ExecuteNext(
			ICommandList* cmdList,
			IRenderPass* renderPass);

		void Flatten();

		UniquePtr<RgBufferManager> m_bufferManager;
		UniquePtr<RgImageManager> m_imageManager;

		RgGeometryManager m_geometryManager;

		std::unordered_map<RenderPassUuid, IRenderPass*> m_renderPassesMap;
		DynamicArray<RenderPassEntry> m_renderPassesEntries{};
		DynamicArray<IRenderPass*> m_flatRenderPass{};

		IRenderer* m_renderer = nullptr;

	};

}
