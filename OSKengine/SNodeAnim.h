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

		~SNodeAnim() {
			//if (PositionKeys)
				//delete[] PositionKeys;
			//if (PositionKeys)
				//delete[] RotationKeys;
			//if (PositionKeys)
				//delete[] ScalingKeys;
		}

		std::string Name = "$NO";

		uint32_t NumberOfPositionKeys = 0;
		SVectorKey* PositionKeys = nullptr;

		uint32_t NumberOfRotationKeys = 0;
		SQuaternionKey* RotationKeys = nullptr;

		uint32_t NumberOfScalingKeys = 0;
		SVectorKey* ScalingKeys = nullptr;

		glm::mat4 Matrix;

	};

	struct OSKAPI_CALL SNode {
		~SNode() {
			//if (Children)
				//delete[] Children;
		}

		std::string Name;
		glm::mat4 Matrix = glm::mat4(1.0f);

		SNode* Children = nullptr;
		uint32_t NumberOfChildren = 0;
	};

}
