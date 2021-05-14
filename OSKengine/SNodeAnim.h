#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SVectorKey.h"
#include "SQuaternionKey.h"

#include <vector>

namespace OSK::Animation {

	/// <summary>
	/// Representa un nodo animado:
	/// sus keyframes (y sus valores)
	/// y su matriz.
	/// </summary>
	struct OSKAPI_CALL SNodeAnim {

		/// <summary>
		/// Elimina el nodo.
		/// </summary>
		void Clear();

		/// <summary>
		/// Nombre del nodo.
		/// </summary>
		std::string name = "$NO";

		/// <summary>
		/// Keyframes de posición.
		/// </summary>
		std::vector<SVectorKey> positionKeys;

		/// <summary>
		/// Keyframes de rotación.
		/// </summary>
		std::vector<SQuaternionKey> rotationKeys;

		/// <summary>
		/// Keyframes de escala.
		/// </summary>
		std::vector<SVectorKey> scalingKeys;

		/// <summary>
		/// Matriz del nodo animado.
		/// </summary>
		glm::mat4 matrix;

	};

}
