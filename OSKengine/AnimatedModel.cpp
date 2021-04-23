#include "AnimatedModel.h"
#include "ToString.h"

using namespace OSK::Animation;

namespace OSK {

	AnimatedModel::~AnimatedModel() {
		for (uint32_t i = 0; i < NumberOfAnimations; i++)
			Animations[i].Clear();

		Memory::SafeDeleteArray(&Animations);
	}

	void AnimatedModel::SetAnimation(uint32_t animID) {
		CurrentAnimation = &Animations[animID];
	}

	void AnimatedModel::SetAnimation(const std::string& name) {
		for (uint32_t i = 0; i < NumberOfAnimations; i++) {
			if (Animations[i].Name == name) {
				CurrentAnimation = &Animations[i];

				return;
			}
		}
	}

	glm::mat4 AnimatedModel::AiToGLM(const aiMatrix4x4& from) {
		return glm::transpose(glm::make_mat4(&from.a1));
	}

	void AnimatedModel::SetupAnimationIndices() {
		SetupAnimationIndices(&RootNode);
	}

	void AnimatedModel::Update(float deltaTime) {
		time -= deltaTime * AnimationSpeed;
		float TicksPerSecond = (float)(CurrentAnimation->TicksPerSecond != 0 ? CurrentAnimation->TicksPerSecond : 25.0f);
		float TimeInTicks = time * TicksPerSecond;

		if (TimeInTicks > (float)CurrentAnimation->Duration) {
			time = 0;
			TimeInTicks = 0;
		}
		if (TimeInTicks <= 0) {
			TimeInTicks = (float)CurrentAnimation->Duration + TimeInTicks;
			time = TimeInTicks / TicksPerSecond;
		}

		ReadNodeHierarchy(TimeInTicks, RootNode, glm::mat4(1.0f));

		for (uint32_t i = 0; i < BoneInfos.size(); i++) {
			BonesUBOdata.Bones[i] = BoneInfos[i].FinalTransformation;
		}
	}

	SNodeAnim AnimatedModel::FindNodeAnim(const SAnimation* animation, const std::string& nodeName) {
		for (uint32_t i = 0; i < animation->NumberOfChannels; i++) {
			SNodeAnim node = animation->BoneChannels[i];
			if (node.Name == nodeName)
				return node;
		}

		return SNodeAnim{};
	}

	Vector3f AnimatedModel::InterpolateVectors(const Vector3f& vec1, const Vector3f& vec2, float delta) const {
		return vec1 + (vec2 - vec1) * delta;
	}

	glm::mat4 AnimatedModel::GetPosition(float time, const SNodeAnim& node)  const {
		Vector3f translation;
		if (node.NumberOfPositionKeys == 1)
			translation = node.PositionKeys[0].Value;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node.NumberOfPositionKeys - 1; i++) {
				if (time < (float)node.PositionKeys[i + 1].Time) {
					frameIndex = i;
					break;
				}
			}

			SVectorKey currentFrame = node.PositionKeys[frameIndex];
			SVectorKey nextFrame = node.PositionKeys[(frameIndex + 1) % node.NumberOfPositionKeys];

			float delta = (time - (float)currentFrame.Time) / (float)(nextFrame.Time - currentFrame.Time);

			Vector3f start = currentFrame.Value;
			Vector3f end = nextFrame.Value;

			translation = InterpolateVectors(start, end, delta);
		}

		glm::mat4 mat(1.0f);
		mat = glm::translate(mat, translation.ToGLM());

		return mat;
	}

	glm::mat4 AnimatedModel::GetRotation(float time, const SNodeAnim& node) const {
		Quaternion rotation;
		if (node.NumberOfRotationKeys == 1)
			rotation = node.RotationKeys[0].Value;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node.NumberOfRotationKeys - 1; i++) {
				if (time < (float)node.RotationKeys[i + 1].Time) {
					frameIndex = i;
					break;
				}
			}

			SQuaternionKey currentFrame = node.RotationKeys[frameIndex];
			SQuaternionKey nextFrame = node.RotationKeys[(frameIndex + 1) % node.NumberOfRotationKeys];

			float delta = (time - (float)currentFrame.Time) / (float)(nextFrame.Time - currentFrame.Time);

			Quaternion& start = currentFrame.Value;
			Quaternion& end = nextFrame.Value;
			
			rotation = start.Interpolate(end, delta);
			rotation.Quat = glm::normalize(rotation.Quat);
		}

		return rotation.ToMat4();
	}

	glm::mat4 AnimatedModel::GetScale(float time, const SNodeAnim& node) const {
		Vector3f scale;
		if (node.NumberOfScalingKeys == 1)
			scale = node.ScalingKeys[0].Value;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node.NumberOfScalingKeys - 1; i++) {
				if (time < (float)node.ScalingKeys[i + 1].Time) {
					frameIndex = i;
					break;
				}
			}

			SVectorKey currentFrame = node.ScalingKeys[frameIndex];
			SVectorKey nextFrame = node.ScalingKeys[(frameIndex + 1) % node.NumberOfScalingKeys];

			float delta = (time - (float)currentFrame.Time) / (float)(nextFrame.Time - currentFrame.Time);

			Vector3f start = currentFrame.Value;
			Vector3f end = nextFrame.Value;

			scale = InterpolateVectors(start, end, delta);
		}

		glm::mat4 mat(1.0f);
		mat = glm::scale(mat, scale.ToGLM());
		
		return mat;
	}

	void AnimatedModel::SetupAnimationIndices(SNode* node) {
		for (uint32_t i = 0; i < CurrentAnimation->NumberOfChannels; i++) {
			if (CurrentAnimation->BoneChannels[i].Name == node->Name) {
				node->SNodeAnimIndex = i;

				break;
			}
		}

		for (uint32_t i = 0; i < node->NumberOfChildren; i++)
			SetupAnimationIndices(&node->Children[i]);
	}

	void AnimatedModel::ReadNodeHierarchy(float animTime, SNode& node, const glm::mat4& parent) {
		glm::mat4 NodeTransformation = glm::mat4(1.0f);

		if (node.SNodeAnimIndex != -1) {
			SNodeAnim pNodeAnim = CurrentAnimation->BoneChannels[node.SNodeAnimIndex];

			glm::mat4 matScale = GetScale(animTime, pNodeAnim);
			glm::mat4 matRotation = GetRotation(animTime, pNodeAnim);
			glm::mat4 matTranslation = GetPosition(animTime, pNodeAnim);

			NodeTransformation = matTranslation * matRotation * matScale;
		}

		glm::mat4 GlobalTransformation = parent * NodeTransformation;

		std::map<std::string, uint32_t>::iterator iterator = BoneMapping.find(node.Name);
		if (iterator != BoneMapping.end()) {
			uint32_t BoneIndex = iterator.operator*().second;
			BoneInfos[BoneIndex].FinalTransformation = GlobalInverseTransform * GlobalTransformation * BoneInfos[BoneIndex].Offset;
		}

		for (uint32_t i = 0; i < node.NumberOfChildren; i++)
			ReadNodeHierarchy(animTime, node.Children[i], GlobalTransformation);
	}

}