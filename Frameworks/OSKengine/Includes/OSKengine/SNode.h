#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Animation {

	/// <summary>
	/// Representa un nodo de un modelo 3D y sus hijos.
	/// </summary>
	struct OSKAPI_CALL SNode {

		/// <summary>
		/// Elimina el nodo.
		/// </summary>
		void Clear();

		/// <summary>
		/// Nombre.
		/// </summary>
		std::string Name;

		/// <summary>
		/// Matriz del nodo.
		/// </summary>
		glm::mat4 Matrix = glm::mat4(1.0f);

		/// <summary>
		/// Número de nodos hijos.
		/// </summary>
		uint32_t NumberOfChildren = 0;

		/// <summary>
		/// Nodos hijos.
		/// </summary>
		SNode* Children = nullptr;

		/// <summary>
		/// Posición de este nodo en el array de nodos del modelo 3D al que pertenece.
		/// </summary>
		int SNodeAnimIndex = -1;

	};


}