#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK::Collision {

	//Representa la proyecci�n de un OBB sobre un eje.
	class OSKAPI_CALL SAT_Projection {

	public:

		//Inicia los valores de una proyecci�n.
		//	<min>: valor m�nimo de la proyecci�n.
		//	<max>: valor m�ximo de la proyecci�n.
		inline SAT_Projection(float min, float max) {
			this->min = min;
			this->max = max;
		}

		//Devuelve true si una proyecci�n toca a otra.
		//Solo debe usarse con proyecciones sobre un mismo eje.
		//	<proj>: otra proyecci�n.
		inline bool Overlaps(const SAT_Projection& proj) const {
			return max > proj.min && min < proj.max;
		}

		//Devuelve la profundidad del overlap de dos proyecciones.
		//Solo debe usarse con proyecciones sobre un mismo eje.
		//	<proj>: otra proyecci�n.
		inline float GetOverlap(const SAT_Projection& proj) {
			if (!Overlaps(proj))
				return 0.0f;

			float output = 0.0f;
			output = max - proj.min;

			const float out = min - proj.max;
			if (std::abs(out) < std::abs(output))
				output = out;

			return output;
		}

	private:

		float min;
		float max;

	};

}
