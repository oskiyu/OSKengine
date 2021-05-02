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
		/// N�mero de keyframes de posici�n.
		/// </summary>
		uint32_t NumberOfPositionKeys = 0;

		/// <summary>
		/// Keyframes de posici�n.
		/// </summary>
		SVectorKey* PositionKeys = nullptr;

		/// <summary>
		/// N�mero de keyframes de rotaci�n.
		/// </summary>
		uint32_t NumberOfRotationKeys = 0;

		/// <summary>
		/// Keyframes de rotaci�n.
		/// </summary>
		SQuaternionKey* RotationKeys = nullptr;

		/// <summary>
		/// N�mero de keyframes de escala.
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
