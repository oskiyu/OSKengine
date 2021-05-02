#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Quaternion.h"

namespace OSK::Animation {

	/// <summary>
	/// Valor quaternial de un keyframe.
	/// </summary>
	struct OSKAPI_CALL SQuaternionKey {

		/// <summary>
		/// Keyframe.
		/// </summary>
		deltaTime_t Time;

		/// <summary>
		/// Valor.
		/// </summary>
		Quaternion Value;

	};

}