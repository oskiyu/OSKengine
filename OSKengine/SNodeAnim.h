#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Quaternion.h"

#include <vector>

namespace OSK::Animation {

	struct OSKAPI_CALL SVectorKey {
		deltaTime_t Time;
		Vector3f Value;
	};

	struct OSKAPI_CALL SQuaternionKey {
		deltaTime_t Time;
		Quaternion Value;
	};

	struct OSKAPI_CALL SNodeAnim {
		std::string Name;

		uint32_t NumberOfPositionKeys;
		std::vector<SVectorKey> PositionKeys;

		uint32_t NumberOfRotationKeys;
		std::vector<SQuaternionKey> RotationKeys;

		uint32_t NumberOfScalingKeys;
		std::vector<SVectorKey> ScalingKeys;

		glm::mat4 Matrix;

	};

	struct OSKAPI_CALL SNode {
		std::string Name;
		glm::mat4 Matrix = glm::mat4(1.0f);

		std::vector<SNode> Children;
	};

}
