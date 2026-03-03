#pragma once

#include <span>

#include "ApiCall.h"
#include "DynamicArray.hpp"

#include "Material.h"
#include "IMaterialSlot.h"
#include "MaterialInstance.h"

#include "RgBufferDependency.h"
#include "RgImageDependency.h"

#include "ICommandList.h"

#include <optional>

#include "Uuid.h"
#include "ResourcesInFlight.h"

namespace OSK::GRAPHICS {

	class RenderGraph;

	enum class RenderPassType {
		GRAPHICS,
		COMPUTE,
		RAY_TRACING
	};

	/// @brief Render-pass dentro de un render-graph.
	class OSKAPI_CALL IRenderPass {

	protected:

		/// @brief Inicializa el render-pass.
		/// @param materialName Nombre del material que usará el renderpass.
		/// @param type Tipo de renderpass.
		/// 
		/// @post Tendrá una instancia de material creada.
		explicit IRenderPass(
			std::string_view materialName,
			RenderPassType type);

	public:

		virtual ~IRenderPass() = default;

		OSK_DEFINE_AS(IRenderPass);


		/// @brief Ejecuta el renderpass.
		/// 
		/// @param cmdList Lista de comandos.
		/// @param renderGraph Render-graph que invoca el renderpass.
		/// Para tener acceso a los recursos GPU.
		/// 
		/// @note Enlaza todos los recursos antes de ejecutar la
		/// lógica de renderizado.
		void _Execute(
			ICommandList* cmdList,
			RenderGraph& renderGraph);

		/// @brief Lógica de renderizado del renderpass.
		/// @param cmdList Lista de comandos.
		virtual void Execute(ICommandList* cmdList) = 0;


		/// @return Dependencias completas con buffers.
		std::span<const RgBufferDependency> GetBufferDependencies() const;

		/// @return Dependencias completas con imágenes.
		std::span<const RgImageDependency>  GetImageDependencies() const;


		/// @return Dependencias parciales con buffers.
		std::span<const PartialRgBufferDependency> GetPartialBufferDependencies() const;

		/// @return Dependencias parciales con imágenes.
		std::span<const PartialRgImageDependency>  GetPartialImageDependencies() const;


		/// @brief Añade una nueva dependencia completa de buffer.
		/// @param dependency Nueva dependencia.
		/// @post También se añade a la lista de dependencias parciales.
		void AddBufferDependency(const RgBufferDependency& dependency);

		/// @brief Añade una nueva dependencia completa de imagen.
		/// @param dependency Nueva dependencia.
		/// @post También se añade a la lista de dependencias parciales.
		void AddImageDependency(const RgImageDependency& dependency);


		/// @brief Añade una dependencia parcial de buffer.
		/// @param dependency Dependencia parcial.
		void AddPartialBufferDependency(const PartialRgBufferDependency& dependency);

		/// @brief Añade una dependencia parcial de imagen.
		/// @param dependency Dependencia parcial.
		void AddPartialImageDependency (const PartialRgImageDependency& dependency);

		RenderPassType GetType() const;

		/// @return Nombre ÚNICO del renderpass.
		virtual std::string_view GetName() const = 0;

	private:

		/// @brief Enlaza todas las dependencias a las
		/// instancias de materiales del pase de renderizado.
		/// 
		/// @param cmdList Lista de comandos.
		/// @param renderGraph Grafo de renderizado.
		/// 
		/// @note En caso de que haya una dependencia de 
		/// buffer de índices, se enlaza a @p cmdList.
		/// @todo Enlazar lista de vértices normales.
		void BindResources(
			ICommandList* cmdList,
			RenderGraph& renderGraph);

		/// @brief Enlaza los buffers de vértices y de índices
		/// indicados en las dependencias de buffers.
		/// 
		/// @param cmdList Lista de comandos.
		/// @param renderGraph Grafo de renderizado.
		/// 
		/// @pre El renderpass debe ser de tipo gráfico.
		/// 
		/// @todo Buffer de vértices.
		void RebindGraphicsBuffers(
			ICommandList* cmdList,
			RenderGraph& renderGraph);

		/// @brief Establece las imágenes de
		/// renderizado de color y profundidad.
		/// 
		/// @param renderGraph Grafo de renderizado.
		/// 
		/// @pre El renderpass debe ser de tipo gráfico.
		void BindTargetImages(RenderGraph& renderGraph);

		bool m_needsResourceBinding = false;

		// Los rangos de las dependencias establecidas
		// en el constructor no tienen establecidos los
		// rangos. Este flag indica si se han establecido
		// dichos rangos.
		bool m_hasFullyRegisteredDependencies = false;

		Material* m_material;
		std::array<UniquePtr<MaterialInstance>, MAX_RESOURCES_IN_FLIGHT> m_materialInstance;

		DynamicArray<PartialRgBufferDependency> m_partialBufferDependencies;
		DynamicArray<PartialRgImageDependency>  m_partialImageDependencies;

		DynamicArray<RgBufferDependency> m_bufferDependencies;
		DynamicArray<RgImageDependency>  m_imageDependencies;

		RenderPassType m_type;

		std::optional<DynamicArray<RenderPassImageInfo>> m_colorTargets;
		std::optional<RenderPassImageInfo> m_depthTarget;

	};


	/// @brief Identificador único para un renderpass
	/// dentro de un rendergraph.
	using RenderPassUuid = BaseUuid<class RgRenderPass>;

}

OSK_DEFINE_UUID_HASH(OSK::GRAPHICS::RenderPassUuid);
OSK_DEFINE_UUID_FORMATTER(OSK::GRAPHICS::RenderPassUuid, "RenderPass ID: ");
