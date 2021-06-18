#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SNode.h"
#include "SNodeAnim.h"

#include <vector>

namespace OSK::Animation {

	/// <summary>
	/// Estructura que representa una animaci�n de un modelo 3D.
	/// </summary>
	struct OSKAPI_CALL SAnimation {

		/// <summary>
		/// Elimina la animaci�n.
		/// </summary>
		void Clear();

		/// <summary>
		/// Nombre de la animaci�n.
		/// </summary>
		std::string name;

		/// <summary>
		/// Duraci�n de la animaci�n.
		/// </summary>
		deltaTime_t duration;

		/// <summary>
		/// Ticks de la animaci�n, por segundo.
		/// </summary>
		deltaTime_t ticksPerSecond;

		/// <summary>
		/// Nodos animados.
		/// </summary>
		std::vector<SNodeAnim> boneChannels;

	};

}
