#include "AnimatedModel.h"
#include "ToString.h"

using namespace OSK::Animation;

namespace OSK {

	AnimatedModel::~AnimatedModel() {
		for (uint32_t i = 0; i < animations.size(); i++)
			animations[i].Clear();
	}

	void AnimatedModel::SetAnimation(uint32_t animID) {
		currentAnimation = &animations[animID];
	}

	void AnimatedModel::SetAnimation(const std::string& name) {
		for (uint32_t i = 0; i < animations.size(); i++) {
			if (animations[i].name == name) {
				currentAnimation = &animations[i];

				return;
			}
		}
	}

	glm::mat4 AnimatedModel::AiToGLM(const aiMatrix4x4& from) {
		return glm::transpose(glm::make_mat4(&from.a1));
	}

	void AnimatedModel::SetupAnimationIndices() {
		SetupAnimationIndices(&rootNode);
	}

	void AnimatedModel::Update(float deltaTime) {
		time -= deltaTime * animationSpeed;
		float ticksPerSecond = (float)(currentAnimation->ticksPerSecond != 0 ? currentAnimation->ticksPerSecond : 25.0f);
		float timeInTicks = time * ticksPerSecond;

		if (timeInTicks > (float)currentAnimation->duration) {
			time = 0;
			timeInTicks = 0;
		}
		if (timeInTicks <= 0) {
			timeInTicks = (float)currentAnimation->duration + timeInTicks;
			time = timeInTicks / ticksPerSecond;
		}

		ReadNodeHierarchy(timeInTicks, rootNode, glm::mat4(1.0f));

		for (uint32_t i = 0; i < boneInfos.size(); i++) {
			bonesUBOdata.bones[i] = boneInfos[i].finalTransformation;
		}
	}

	SNodeAnim AnimatedModel::FindNodeAnim(const SAnimation* animation, const std::string& nodeName) {
		for (uint32_t i = 0; i < animation->boneChannels.size(); i++) {
			SNodeAnim node = animation->boneChannels[i];
			if (node.name == nodeName)
				return node;
		}

		return SNodeAnim{};
	}

	Vector3f AnimatedModel::InterpolateVectors(const Vector3f& vec1, const Vector3f& vec2, float delta) const {
		return vec1 + (vec2 - vec1) * delta;
	}

	glm::mat4 AnimatedModel::GetPosition(float time, const SNodeAnim& node)  const {
		Vector3f translation;
		if (node.positionKeys.size() == 1)
			translation = node.positionKeys[0].value;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node.positionKeys.size() - 1; i++) {
				if (time < (float)node.positionKeys[i + 1].time) {
					frameIndex = i;
					break;
				}
			}

			SVectorKey currentFrame = node.positionKeys[frameIndex];
			SVectorKey nextFrame = node.positionKeys[(frameIndex + 1) % node.positionKeys.size()];

			float delta = (time - (float)currentFrame.time) / (float)(nextFrame.time - currentFrame.time);

			Vector3f start = currentFrame.value;
			Vector3f end = nextFrame.value;

			translation = InterpolateVectors(start, end, delta);
		}

		glm::mat4 mat(1.0f);
		mat = glm::translate(mat, translation.ToGLM());

		return mat;
	}

	glm::mat4 AnimatedModel::GetRotation(float time, const SNodeAnim& node) const {
		Quaternion rotation;
		if (node.rotationKeys.size() == 1)
			rotation = node.rotationKeys[0].value;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node.rotationKeys.size() - 1; i++) {
				if (time < (float)node.rotationKeys[i + 1].time) {
					frameIndex = i;
					break;
				}
			}

			SQuaternionKey currentFrame = node.rotationKeys[frameIndex];
			SQuaternionKey nextFrame = node.rotationKeys[(frameIndex + 1) % node.rotationKeys.size()];

			float delta = (time - (float)currentFrame.time) / (float)(nextFrame.time - currentFrame.time);

			Quaternion& start = currentFrame.value;
			Quaternion& end = nextFrame.value;
			
			rotation = start.Interpolate(end, delta);
			rotation.Quat = glm::normalize(rotation.Quat);
		}

		return rotation.ToMat4();
	}

	glm::mat4 AnimatedModel::GetScale(float time, const SNodeAnim& node) const {
		Vector3f scale;
		if (node.scalingKeys.size() == 1)
			scale = node.scalingKeys[0].value;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node.scalingKeys.size() - 1; i++) {
				if (time < (float)node.scalingKeys[i + 1].time) {
					frameIndex = i;
					break;
				}
			}

			SVectorKey currentFrame = node.scalingKeys[frameIndex];
			SVectorKey nextFrame = node.scalingKeys[(frameIndex + 1) % node.scalingKeys.size()];

			float delta = (time - (float)currentFrame.time) / (float)(nextFrame.time - currentFrame.time);

			Vector3f start = currentFrame.value;
			Vector3f end = nextFrame.value;

			scale = InterpolateVectors(start, end, delta);
		}

		glm::mat4 mat(1.0f);
		mat = glm::scale(mat, scale.ToGLM());
		
		return mat;
	}

	void AnimatedModel::SetupAnimationIndices(SNode* node) {
		for (uint32_t i = 0; i < currentAnimation->boneChannels.size(); i++) {
			if (currentAnimation->boneChannels[i].name == node->name) {
				node->sNodeAnimIndex = i;

				break;
			}
		}

		for (uint32_t i = 0; i < node->children.size(); i++)
			SetupAnimationIndices(&node->children[i]);
	}

	void AnimatedModel::ReadNodeHierarchy(float animTime, SNode& node, const glm::mat4& parent) {
		glm::mat4 NodeTransformation = glm::mat4(1.0f);

		if (node.sNodeAnimIndex != -1) {
			SNodeAnim pNodeAnim = currentAnimation->boneChannels[node.sNodeAnimIndex];

			glm::mat4 matScale = GetScale(animTime, pNodeAnim);
			glm::mat4 matRotation = GetRotation(animTime, pNodeAnim);
			glm::mat4 matTranslation = GetPosition(animTime, pNodeAnim);

			NodeTransformation = matTranslation * matRotation * matScale;
		}

		glm::mat4 globalTransformation = parent * NodeTransformation;

		std::map<std::string, uint32_t>::iterator iterator = boneMapping.find(node.name);
		if (iterator != boneMapping.end()) {
			uint32_t BoneIndex = iterator.operator*().second;
			boneInfos[BoneIndex].finalTransformation = globalInverseTransform * globalTransformation * boneInfos[BoneIndex].offset;
		}

		for (uint32_t i = 0; i < node.children.size(); i++)
			ReadNodeHierarchy(animTime, node.children[i], globalTransformation);
	}

}