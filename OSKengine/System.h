#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "ComponentArray.h"
#include "Component.h"

#include <set>
#include <vulkan/vulkan.h>

namespace OSK {
	class EntityComponentSystem;
}

namespace OSK::ECS {

	/// <summary>
	/// Sistema del ECS.
	/// Maneja los objetos y componentes de objetos que tengan un Signature dado.
	/// </summary>
	class System {

		friend class EntityComponentSystem;
		friend class SystemManager;

	public:

		/// <summary>
		/// Destructor.
		/// </summary>
		virtual ~System() = default;

		/// <summary>
		/// Se ejecuta al crearse el sistema.
		/// </summary>
		virtual void OnCreate() {}

		/// <summary>
		/// Se ejecuta una vez por tick.
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		virtual void OnTick(deltaTime_t deltaTime) {}

		/// <summary>
		/// Se ejecuta una vez por tick.
		/// Para el sistema de renderizado.
		/// </summary>
		/// <param name="cmdBuffer">Buffer de comandos de Vulkan.</param>
		/// <param name="i">Iteraci�n.</param>
		virtual void OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) {}

		/// <summary>
		/// Se ejecuta al eliminarse el sistema.
		/// </summary>
		virtual void OnRemove() {}

		/// <summary>
		/// Obtiene el Signature (componentes necesarios de un objeto) del sistema.
		/// </summary>
		/// <returns>Signature.</returns>
		virtual Signature GetSystemSignature();

	protected:

		/// <summary>
		/// Objetos manejados por el sistema.
		/// </summary>
		std::set<GameObjectID> objects;

		/// <summary>
		/// Sistema ECS al que pertenece.
		/// </summary>
		EntityComponentSystem* entityComponentSystem = nullptr;

	};
	
}
