#include "Bone.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/euler_angles.hpp>

namespace OSK {

	Bone::Bone(int32_t ID, std::string name) {
		ID = ID;
		Name = name;

		Transform = OSK::Transform();

		AINode = nullptr;
		AINodeAnim = nullptr;
	}


	Bone::~Bone() {

	}


	void Bone::AttachTo(Bone& bone) {
		Transform.AttachTo(&bone.Transform);
	}


	void Bone::UpdateTransform(const float_t& time) {
		if (!AINodeAnim) {
			return;
		}

		Transform.Position = GetInterpolatedPosition(time);
		Transform.Rotation = GetInterpolatedRotation(time);
	}


	Vector3 Bone::GetInterpolatedPosition(const float_t& time) {
		Vector3 out;

		if (AINodeAnim->mNumPositionKeys == 1) {
			aiVector3D help = AINodeAnim->mPositionKeys[0].mValue;

			out = Vector3(help.x, help.y, help.z);

			return out;
		}

		int32_t positionIndex = GetPositionKeyframe(time);
		int32_t nextPositionIndex = positionIndex + 1;

		float_t deltaTime = static_cast<float_t>(AINodeAnim->mPositionKeys[nextPositionIndex].mTime - AINodeAnim->mPositionKeys[positionIndex].mTime);
		float_t factor = (time - static_cast<float_t>(AINodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime);
	
		aiVector3D begin = AINodeAnim->mPositionKeys[positionIndex].mValue;
		aiVector3D end = AINodeAnim->mPositionKeys[nextPositionIndex].mValue;

		Vector3 p1 = Vector3(begin.x, begin.y, begin.z);
		Vector3 p2 = Vector3(end.x, end.y, end.z);
		
		out = glm::mix(p1.ToGLM(), p2.ToGLM(), factor);

		return out;
	}


	Vector3 Bone::GetInterpolatedRotation(const float_t& time) {
		if (AINodeAnim->mNumRotationKeys == 1) {
			aiQuaternion help = AINodeAnim->mRotationKeys[0].mValue;

			glm::quat out = glm::quat(help.w, help.x, help.y, help.z);

			return Vector3(glm::eulerAngles(out) * 3.14159f / 180.f);
		}

		int32_t rotationIndex = GetRotationKeyframe(time);
		int32_t nextRotationIndex = rotationIndex + 1;

		float_t deltaTime = static_cast<float_t>(AINodeAnim->mPositionKeys[nextRotationIndex].mTime - AINodeAnim->mPositionKeys[rotationIndex].mTime);
		float_t factor = (time - static_cast<float_t>(AINodeAnim->mPositionKeys[rotationIndex].mTime) / deltaTime);

		aiQuaternion begin = AINodeAnim->mRotationKeys[rotationIndex].mValue;
		aiQuaternion end = AINodeAnim->mRotationKeys[nextRotationIndex].mValue;

		glm::quat r1 = glm::quat(begin.w, begin.x, begin.y, begin.z);
		glm::quat r2 = glm::quat(end.w, end.x, end.y, end.z);

		glm::quat out = glm::slerp(r1, r2, factor);

		return Vector3(glm::eulerAngles(out) * (3.14f));
	}


	uint32_t Bone::GetPositionKeyframe(const float_t& time) {
		for (uint32_t i = 0; i < AINodeAnim->mNumPositionKeys - 1; i++) {
			if (time < AINodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}

		//OSK::Log("ERROR: no se encuentra keyframe para la posición de tiempo: " + std::to_string(time), __LINE__);
		return 0;
	}


	uint32_t Bone::GetRotationKeyframe(const float_t& time) {
		for (uint32_t i = 0; i < AINodeAnim->mNumRotationKeys - 1; i++) {
			if (time < AINodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		//OSK::Log("ERROR: no se encuentra keyframe para la rotación de tiempo: " + std::to_string(time), __LINE__);
		return 0;
	}

}