#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "OldMaterial.h"

#include <string>
#include <list>

namespace OSK {

	class RenderAPI;

	/// <summary>
	/// Sistema de materiales:
	/// clase que maneja los materiales del juego.
	/// Permite registrar materiales, y registrar renderpasses para su uso con materiales.
	/// </summary>
	class OSKAPI_CALL OldMaterialSystem {

	public:

		/// <summary>
		/// Crea el material system.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		OldMaterialSystem(const RenderAPI* renderer);

		/// <summary>
		/// Elimina el material system.
		/// Elimina todos los materiales.
		/// </summary>
		~OldMaterialSystem();

		/// <summary>
		/// Registra un nuevo material.
		/// </summary>
		/// <param name="name">Nombre del material.</param>
		void RegisterMaterial(const std::string& name);

		/// <summary>
		/// Obtiene el material.
		/// </summary>
		/// <param name="name">Nombre del material.</param>
		/// <returns>Material.</returns>
		OldMaterial* GetMaterial(const std::string& name);

		/// <summary>
		/// Genera graphics pipelines de los materiales,
		/// para que puedan usarse los materiales en ellos.
		/// </summary>
		/// <param name="renderpass">Renderpass para el que se van a crear graphics pipelines.</param>
		void RegisterRenderpass(VULKAN::Renderpass* renderpass);

		/// <summary>
		/// Elimina los graphics pipelines enlazados al renderpass.
		/// </summary>
		/// <param name="renderpass">Renderpass para el que se van a elimnar graphics pipelines.</param>
		void UnregisterRenderpass(VULKAN::Renderpass* renderpass);

	private:

		/// <summary>
		/// Materiales.
		/// </summary>
		std::unordered_map<std::string, OldMaterial*> materials;

		/// <summary>
		/// Renderpasses enlazados a los materiales.
		/// </summary>
		std::list<VULKAN::Renderpass*> renderpasses;

		/// <summary>
		/// Renderizador.
		/// </summary>
		const RenderAPI* renderer = nullptr;

	};

}
