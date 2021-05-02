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
		std::string Name = "$NO";

		/// <summary>
		/// Número de keyframes de posición.
		/// </summary>
		uint32_t NumberOfPositionKeys = 0;

		/// <summary>
		/// Keyframes de posición.
		/// </summary>
		SVectorKey* PositionKeys = nullptr;

		/// <summary>
		/// Número de keyframes de rotación.
		/// </summary>
		uint32_t NumberOfRotationKeys = 0;

		/// <summary>
		/// Keyframes de rotación.
		/// </summary>
		SQuaternionKey* RotationKeys = nullptr;

		/// <summary>
		/// Número de keyframes de escala.
		/// </summary>
		uint32_t NumberOfScalingKeys = 0;

		/// <summary>
		/// Keyframes de escala.
		/// </summary>
		SVectorKey* ScalingKeys = nullptr;

		/// <summary>
		/// Matriz del nodo animado.
		/// </summary>
		glm::mat4 Matrix;

	};

}
