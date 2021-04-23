#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "SNode.h"
#include "SNodeAnim.h"

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
		std::string Name;

		/// <summary>
		/// Duración de la animación.
		/// </summary>
		deltaTime_t Duration;

		/// <summary>
		/// Ticks de la animación, por segundo.
		/// </summary>
		deltaTime_t TicksPerSecond;

		/// <summary>
		/// Número de nodos animados.
		/// </summary>
		uint32_t NumberOfChannels;

		/// <summary>
		/// Nodos animados.
		/// </summary>
		SNodeAnim* BoneChannels = nullptr;

	};

}
