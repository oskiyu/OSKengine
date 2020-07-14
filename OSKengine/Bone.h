#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Transform.h"

#include <assimp/anim.h>
#include <assimp/scene.h>

namespace OSK {

	//Clase que define un bone para la animación 3D.
	class OSKAPI_CALL Bone {

	public:

		Bone(int32_t ID, std::string name);


		~Bone();


		void AttachTo(Bone& bone);


		void UpdateTransform(const float_t& time);


		Vector3 GetInterpolatedPosition(const float_t& time);


		Vector3 GetInterpolatedRotation(const float_t& time);


		uint32_t GetPositionKeyframe(const float_t& time);


		uint32_t GetRotationKeyframe(const float_t& time);


		Transform Transform;


		glm::mat4 transf{};


		std::string Name;


		int32_t ID;


		float_t Weight;


		aiNodeAnim* AINodeAnim;

		
		aiNode* AINode;


	private:

	};

}
