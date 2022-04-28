#pragma once

#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "OSKmacros.h"
#include "HashMap.hpp"
#include "PipelineCreateInfo.h"
#include "MaterialLayout.h"
#include "VertexInfo.h"

namespace OSK::GRAPHICS {

	class MaterialInstance;
	class IGraphicsPipeline;
	struct PipelineCreateInfo;
	class IRenderpass;

	/// <summary>
	/// Un material define el comportamiento del renderizador con un objeto en concreto.
	/// Debe definirse el layout del material, indicando qu� slots (y bindings) ser�n usados en los shaders.
	/// 
	/// @note Tiene la propiedad de los graphics pipelines (uno por cada renderpass registrado).
	/// </summary>
	class OSKAPI_CALL Material {

	public:

		/// <summary>
		/// Crea un nuevo material.
		/// </summary>
		/// <param name="pipelineInfo">Informaci�n de las caracter�sticas del graphics pipeline.</param>
		/// <param name="layout">Layout del material. @note Este material ser� el due�o del layout.</param>
		Material(const GRAPHICS::PipelineCreateInfo& pipelineInfo, OwnedPtr<MaterialLayout> layout, const VertexInfo& vertexInfo);
		~Material();

		const MaterialLayout* GetLayout() const;

		/// <summary>
		/// Obtiene el graphics pipeline asignado al renderpass dado.
		/// </summary>
		const IGraphicsPipeline* GetGraphicsPipeline(const IRenderpass* ownerRenderpass) const;
		
		/// <summary>
		/// Registra el renderpass, creando una nueva graphics pipeline asignada a dicho renderpass.
		/// </summary>
		void RegisterRenderpass(const IRenderpass* renderpass);
		/// <summary>
		/// Elimina el graphics pipeline asignado a dicho renderpass.
		/// </summary>
		void UnregisterRenderpass(const IRenderpass* renderpass);

		/// <summary>
		/// Crea y devuelve una nueva instancia de este material.
		/// </summary>
		OwnedPtr<MaterialInstance> CreateInstance();

	private:

		/// <summary>
		/// Informaci�n para la generaci�n de los graphics pipelines.
		/// </summary>
		PipelineCreateInfo pipelineInfo{};

		UniquePtr<MaterialLayout> layout;
		HashMap<const GRAPHICS::IRenderpass*, OwnedPtr<GRAPHICS::IGraphicsPipeline>> graphicsPipelines;

		/// <summary>
		/// Informaci�n del v�rtice para generar los pipelines.
		/// </summary>
		const VertexInfo vertexInfo;

	};

}
