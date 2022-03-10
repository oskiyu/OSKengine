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
	/// Un sistema se encarga de ejecutar l�gica con los datos de los componentes.
	/// Los componentes no son m�s que colecciones de datos: la l�gica est� en
	/// los sistemas.
	/// 
	/// Cada sistema tiene un Signature, que indica qu� componentes debe tener un GameObject
	/// para ser procesado por el sistema.
	/// 
	/// Para que una clase pueda ser usada como sistema, debe tener implementado OSK_SYSTEM(className).
	/// Este debe ser un identificador �nico: no pueden haber dos tipos de sistemas con el mismo className.
	/// </summary>
	class OSKAPI_CALL ISystem {

	public:

		~ISystem() = default;

		/// <summary>
		/// Funci�n que se ejecuta al crearse el sistema.
		/// </summary>
		virtual void OnCreate();

		/// <summary>
		/// Funci�n que se ejecuta una vez por frame.
		/// 
		/// La l�gica en la mayor�a de sistemas estar� implementada
		/// en este m�todo.
		/// </summary>
		/// <param name="deltaTime"></param>
		virtual void OnTick(TDeltaTime deltaTime);

		/// <summary>
		/// Funci�n que se ejecuta al destruirse el sistema.
		/// </summary>
		virtual void OnRemove();


		/// <summary>
		/// Funci�n que se ejecuta cuando un GameObject entra en la lista
		/// de objetos procesables por el sistema.
		/// </summary>
		virtual void OnObjectAdded(GameObjectIndex obj);

		/// <summary>
		/// Funci�n que se ejecuta cuando un GameObject sale de la lista
		/// de objetos procesables por el sistema.
		/// </summary>
		virtual void OnObjectRemoved(GameObjectIndex obj);


		/// <summary>
		/// Devuelve el signature del sistema, indicando qu� componentes
		/// necesita.
		/// </summary>
		const Signature& GetSignature() const;

		/// <summary>
		/// A�ade un objeto a la lista de objetos procesables.
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
