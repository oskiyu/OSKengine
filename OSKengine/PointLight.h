#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Color.h"
#include "Transform.h"

namespace OSK {
	
	/// <summary>
	/// Representa una luz puntual.
	/// </summary>
	struct OSKAPI_CALL PointLight {

		/// <summary>
		/// Posición de la luz.
		/// </summary>
		alignas(16) glm::vec3 Position;

		/// <summary>
		/// Color.
		/// </summary>
		alignas(16) glm::vec4 Color;

		/// <summary>
		/// Información de la luz: <para/>
		/// x = Intensidad. <para/>
		/// y = Radio. <para/>
		/// </summary>
		alignas(16) glm::vec4 Information;

	};

}