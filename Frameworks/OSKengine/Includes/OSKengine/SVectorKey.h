#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Animation {

	/// <summary>
	/// Valor vectorial de un keyframe.
	/// </summary>
	struct OSKAPI_CALL SVectorKey {

		/// <summary>
		/// KeyFrame.
		/// </summary>
		deltaTime_t time;

		/// <summary>
		/// Valor.
		/// </summary>
		Vector3f value;

	};

}