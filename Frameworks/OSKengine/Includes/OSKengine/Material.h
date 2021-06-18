#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "MaterialPool.h"
#include "MaterialInstance.h"
#include "MaterialPipelineInfo.h"

#include "GraphicsPipeline.h"
#include "Renderpass.h"
#include "UniquePtr.hpp"

#include <unordered_map>
#include <map>
#include <string>

namespace OSK {

	class RenderAPI;
	class MaterialSystem;

	/// <summary>
	/// Representa un tipo de material en concreto.
	/// Un material contiene: <para/>
	/// Descripci�n del layout de los shaders. <para/>
	/// Descripci�n del pipeline: shaders que se van a usar.  <para/>
	/// </summary>
	class OSKAPI_CALL Material {

		friend class MaterialSystem;

	public:

		/// <summary>
		/// Destructor.
		/// <summary/>
		~Material();

		/// <summary>
		/// Establece el renderizador al que pertenece el material.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		void SetRenderer(RenderAPI* renderer);

		/// <summary>
		/// Establece la informaci�n sobre el pipeline del material (shaders).
		/// </summary>
		/// <param name="info">Informaci�n.</param>
		void SetPipelineSettings(const MaterialPipelineCreateInfo& info);

		/// <summary>
		/// Establece el layout del material.
		/// </summary>
		/// <param name="layout">Layout.</param>
		void SetLayout(MaterialBindingLayout layout);

		/// <summary>
		/// Crea una nueva instancia de un material.
		/// </summary>
		/// <returns>Nueva instancia.</returns>
		MaterialInstance* CreateInstance();

		/// <summary>
		/// Devuelve el binding dentro del shader de un binding espec�fico.
		/// </summary>
		/// <param name="name">Nombre del binding.</param>
		/// <returns>ID del binding.</returns>
		uint32_t GetBindingIndex(const std::string& name);

		/// <summary>
		/// Registra un renderpass, creando un graphics pipeline espec�fico para �l.
		/// </summary>
		/// <param name="renderpass">Renderpass.</param>
		void RegisterRenderpass(VULKAN::Renderpass* renderpass);

		/// <summary>
		/// Quita el renderpass, eliminando el graphics pipeline enlazado a �l.
		/// </summary>
		/// <param name="renderpass"></param>
		void UnregisterRenderpass(VULKAN::Renderpass* renderpass);

		/// <summary>
		/// Devuelve el graphics pipeline enlazado a un renderpass en concreto.
		/// </summary>
		/// <param name="renderpass">Renderpass.</param>
		/// <returns>Pipeline para el renderpass.</returns>
		GraphicsPipeline* GetGraphicsPipeline(VULKAN::Renderpass* renderpass) const;

		/// <summary>
		/// Devuelve la informaci�n de creaci�n del graphics pipeline del material.
		/// </summary>
		/// <returns>Informaci�n.</returns>
		MaterialPipelineCreateInfo GetMaterialGraphicsPipelineInfo() const;

		/// <summary>
		/// Textura por defecto del material.
		/// </summary>
		static Texture* DefaultTexture;

	private:

		/// <summary>
		/// Informaci�n del pipeline.
		/// </summary>
		MaterialPipelineCreateInfo pipelineInfo;

		/// <summary>
		/// Map: nombre del binding -> binding.
		/// </summary>
		std::unordered_map<std::string, uint32_t> bindingNameToBinding{};

		/// <summary>
		/// Map: renderpass -> pipeline enlazado al renderpass.
		/// </summary>
		std::map<VULKAN::Renderpass*, GraphicsPipeline*> pipelines;

		/// <summary>
		/// Lista de renderpasses que a�n no se han registrado.
		/// </summary>
		std::vector<VULKAN::Renderpass*> renderpassesToRegister;

		/// <summary>
		/// Almacena las instancias de los materiales.
		/// </summary>
		UniquePtr<MaterialPool> pool;

		/// <summary>
		/// Descriptor layout del material.
		/// </summary>
		UniquePtr<DescriptorLayout> materialLayout;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

	};

}
