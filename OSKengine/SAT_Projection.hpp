#pragma once

#include "OSKtypes.h"

namespace OSK::Collision {

	//Representa la proyección de un OBB sobre un eje.
	class SAT_Projection {

	public:

		inline SAT_Projection(const float& min, const float& max) {
			this->min = min;
			this->max = max;
		}

		//Devuelve true si una proyección toca a otra.
		//Solo debe usarse con proyecciones sobre un mismo eje.
		//	<proj>: otra proyección.
		inline bool Overlaps(const SAT_Projection& proj) const {
			return max > proj.min && min < proj.max;
		}

		//Devuelve la profundidad del overlap de dos proyecciones.
		//Solo debe usarse con proyecciones sobre un mismo eje.
		//	<proj>: otra proyección.
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
