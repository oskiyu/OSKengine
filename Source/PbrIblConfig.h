#include "Serializer.h"

namespace OSK::GRAPHICS {

	struct PbrIblConfig {

		OSK_SERIALIZABLE();

		alignas(4) float irradianceStrength = 1.0f;
		alignas(4) float specularStrength = 1.0f;
		alignas(4) float radianceStrength = 1.0f;
		alignas(4) float emissiveStrength = 1.0f;

	};

}

OSK_SERIALIZATION(OSK::GRAPHICS::PbrIblConfig);
