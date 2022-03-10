#pragma once

#include "OSKmacros.h"
#include "GameObject.h"
#include "Component.h"
#include "DynamicArray.hpp"

namespace OSK {
	class ICommandList;
}

namespace OSK::ECS{

	/// <summary>
	/// Un sistema se encarga de ejecutar lógica con los datos de los componentes.
	/// Los componentes no son más que colecciones de datos: la lógica está en
	/// los sistemas.
	/// 
	/// Cada sistema tiene un Signature, que indica qué componentes debe tener un GameObject
	/// para ser procesado por el sistema.
	/// 
	/// Para que una clase pueda ser usada como sistema, debe tener implementado OSK_SYSTEM(className).
	/// Este debe ser un identificador único: no pueden haber dos tipos de sistemas con el mismo className.
	/// </summary>
	class OSKAPI_CALL ISystem {

	public:

		~ISystem() = default;

		/// <summary>
		/// Función que se ejecuta al crearse el sistema.
		/// </summary>
		virtual void OnCreate();

		/// <summary>
		/// Función que se ejecuta una vez por frame.
		/// 
		/// La lógica en la mayoría de sistemas estará implementada
		/// en este método.
		/// </summary>
		/// <param name="deltaTime"></param>
		virtual void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Función que se ejecuta al destruirse el sistema.
		/// </summary>
		virtual void OnRemove();


		/// <summary>
		/// Función que se ejecuta cuando un GameObject entra en la lista
		/// de objetos procesables por el sistema.
		/// </summary>
		virtual void OnObjectAdded(GameObjectIndex obj);

		/// <summary>
		/// Función que se ejecuta cuando un GameObject sale de la lista
		/// de objetos procesables por el sistema.
		/// </summary>
		virtual void OnObjectRemoved(GameObjectIndex obj);


		/// <summary>
		/// Devuelve el signature del sistema, indicando qué componentes
		/// necesita.
		/// </summary>
		const Signature& GetSignature() const;

		/// <summary>
		/// Añade un objeto a la lista de objetos procesables.
		/// </summary>
		void AddObject(GameObjectIndex obj);

		/// <summary>
		/// Elimina un objeto de la lista de objetos procesables.
		/// </summary>
		void RemoveObject(GameObjectIndex obj);

	protected:

		void SetSignature(const Signature& signature);
		const DynamicArray<GameObjectIndex>& GetObjects();

	private:

		DynamicArray<GameObjectIndex> processedObjects;
		Signature signature;

	};

}

#ifndef OSK_SYSTEM
#define OSK_SYSTEM(className) const static inline std::string GetSystemName() { return className; }
#endif
