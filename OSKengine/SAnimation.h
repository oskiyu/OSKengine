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
	/// Estructura que representa una animación de un modelo 3D.
	/// </summary>
	struct OSKAPI_CALL SAnimation {

		/// <summary>
		/// Elimina la animación.
		/// </summary>
		void Clear();

		/// <summary>
		/// Nombre de la animación.
		/// </summary>
		std::string name;

		/// <summary>
		/// Duración de la animación.
		/// </summary>
		deltaTime_t duration;

		/// <summary>
		/// Ticks de la animación, por segundo.
		/// </summary>
		deltaTime_t ticksPerSecond;

		/// <summary>
		/// Nodos animados.
		/// </summary>
		std::vector<SNodeAnim> boneChannels;

	};

}
