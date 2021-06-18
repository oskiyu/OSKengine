#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vector>

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
		std::string name;

		/// <summary>
		/// Matriz del nodo.
		/// </summary>
		glm::mat4 matrix = glm::mat4(1.0f);

		/// <summary>
		/// Nodos hijos.
		/// </summary>
		std::vector<SNode> children;

		/// <summary>
		/// Posición de este nodo en el array de nodos del modelo 3D al que pertenece.
		/// </summary>
		int sNodeAnimIndex = -1;

	};


}