#pragma once

#include "OSKmacros.h"
#include "OSKsettings.h"
#include "OSKtypes.h"
#include "Log.h"

#include "System.h"

#include <vulkan/vulkan.h>

namespace OSK::ECS {

	/// <summary>
	/// Maneja los sistemas del ECS.
	/// Ejecuta sus funciones.
	/// Puede registrar y crear nuevos sistemas.
	/// </summary>
	class SystemManager {

	public:

		/// <summary>
		/// Ejecuta la funcion OnTick de todos los sistemas.
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		void OnTick(deltaTime_t deltaTime);

		/// <summary>
		/// Ejecuta la función OnDraw de todos los sistemas.
		/// </summary>
		/// <param name="cmdBuffer">Buffer de comandos de Vulkan.</param>
		/// <param name="ix">Iteración.</param>
		void OnDraw(VkCommandBuffer cmdBuffer, uint32_t ix);

		/// <summary>
		/// Registra y crea un nuevo sistema. 
		/// </summary>
		/// <typeparam name="T">Clase del sistema.</typeparam>
		/// <returns>Sistema ya creado.</returns>
		template <typename T> T* CreateSystem() {
			const char* name = typeid(T).name();

			T* sistema = new T();
			systems.insert({ name, (System*)sistema });

			return sistema;
		}

		/// <summary>
		/// Establece el signature de un sistema en concrreto.
		/// </summary>
		/// <typeparam name="T">Clase del sistema.</typeparam>
		/// <param name="signature">Signature.</param>
		template <typename T> void SetSignature(Signature signature) {
			const char* name = typeid(T).name();

			signatures.insert({ name, signature });
		}

		/// <summary>
		/// Cuando un objeto ha sido eliminado:
		/// se quita de los sistemas que lo manejen.
		/// </summary>
		/// <param name="obj">ID del objeto.</param>
		void GameObjectDestroyed(GameObjectID obj);
		
		/// <summary>
		/// Cuando un objeto cambia su signature (se añaden o quitan componentes):
		/// se comprueba que está manejado sólamente por los sistemas indicados, 
		/// y por todos los sistemas indicados.
		/// </summary>
		void GameObjectSignatureChanged(GameObjectID object, Signature signature);

	private:

		/// <summary>
		/// Map nombre de sistema -> singature.
		/// </summary>
		std::unordered_map<const char*, Signature> signatures;

		/// <summary>
		/// Map nombre de sistema -> sistema.
		/// </summary>
		std::unordered_map<const char*, System*> systems;

	};

}