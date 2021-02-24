#include "AnimatedModel.h"
#include "ToString.h"

namespace OSK {

	void VertexBoneData::Add(uint32_t id, float weight) {
		for (uint32_t i = 0; i < OSK_ANIM_MAX_BONES_PER_VERTEX; i++) {
			if (Weights[i] == 0.0f) {
				IDs[i] = id;
				Weights[i] = weight;

				return;
			}
		}
	}


	BoneInfo::BoneInfo() {
		Offset = aiMatrix4x4();
		FinalTransformation = aiMatrix4x4();
	}


	void AnimatedModel::Update(float deltaTime) {
		if (Animation == nullptr)
			return;

		time += deltaTime;
		float TicksPerSecond = (float)(Animation->mTicksPerSecond != 0 ? Animation->mTicksPerSecond : 25.0f);
		float TimeInTicks = time * TicksPerSecond;
		if (TimeInTicks > (float)Animation->mDuration) {
			time = 0;
			TimeInTicks = 0;
		}

		aiMatrix4x4 identity = aiMatrix4x4();
		ReadNodeHierarchy(TimeInTicks, scene->mRootNode, identity);

		for (uint32_t i = 0; i < BoneTransforms.size(); i++) {
			BoneTransforms[i] = BoneInfos[i].FinalTransformation;
			BonesUBOdata.Bones[i] = AiToGLM(BoneTransforms[i]);
		}
	}

	const aiNodeAnim* AnimatedModel::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName) const {
		for (uint32_t i = 0; i < animation->mNumChannels; i++) {
			const aiNodeAnim* node = animation->mChannels[i];
			if (std::string(node->mNodeName.data) == nodeName)
				return node;
		}

		return nullptr;
	}

	aiMatrix4x4 AnimatedModel::InterpolateTranslation(float time, const aiNodeAnim* node)  const {
		aiVector3D translation;
		if (node->mNumPositionKeys == 1)
			translation = node->mPositionKeys[0].mValue;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node->mNumPositionKeys - 1; i++) {
				if (time < (float)node->mPositionKeys[i + 1].mTime) {
					frameIndex = i;
					break;
				}
			}

			aiVectorKey currentFrame = node->mPositionKeys[frameIndex];
			aiVectorKey nextFrame = node->mPositionKeys[(frameIndex + 1) % node->mNumPositionKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiVector3D& start = currentFrame.mValue;
			const aiVector3D& end = nextFrame.mValue;

			translation = (start + delta * (end - start));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Translation(translation, mat);

		return mat;
	}

	aiMatrix4x4 AnimatedModel::InterpolateRotation(float time, const aiNodeAnim* node) const {
		aiQuaternion rotation;
		if (node->mNumRotationKeys == 1)
			rotation = node->mRotationKeys[0].mValue;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node->mNumRotationKeys - 1; i++) {
				if (time < (float)node->mRotationKeys[i + 1].mTime) {
					frameIndex = i;
					break;
				}
			}

			aiQuatKey currentFrame = node->mRotationKeys[frameIndex];
			aiQuatKey nextFrame = node->mRotationKeys[(frameIndex + 1) % node->mNumRotationKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiQuaternion& start = currentFrame.mValue;
			const aiQuaternion& end = nextFrame.mValue;

			aiQuaternion::Interpolate(rotation, start, end, delta);
			rotation.Normalize();
		}

		aiMatrix4x4 mat(rotation.GetMatrix());
		return mat;
	}

	aiMatrix4x4 AnimatedModel::InterpolateScale(float time, const aiNodeAnim* node) const {
		aiVector3D scale;
		if (node->mNumScalingKeys == 1)
			scale = node->mScalingKeys[0].mValue;
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < node->mNumScalingKeys - 1; i++) {
				if (time < (float)node->mScalingKeys[i + 1].mTime) {
					frameIndex = i;
					break;
				}
			}

			aiVectorKey currentFrame = node->mScalingKeys[frameIndex];
			aiVectorKey nextFrame = node->mScalingKeys[(frameIndex + 1) % node->mNumScalingKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const aiVector3D& start = currentFrame.mValue;
			const aiVector3D& end = nextFrame.mValue;

			scale = (start + delta * (end - start));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Scaling(scale, mat);
		return mat;
	}

	void AnimatedModel::ReadNodeHierarchy(float animTime, const aiNode* node, const aiMatrix4x4& parent) {
		std::string NodeName(node->mName.data);

		aiMatrix4x4 NodeTransformation(node->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(Animation, NodeName);

		if (pNodeAnim) {
			aiMatrix4x4 matScale = InterpolateScale(animTime, pNodeAnim);
			aiMatrix4x4 matRotation = InterpolateRotation(animTime, pNodeAnim);
			aiMatrix4x4 matTranslation = InterpolateTranslation(animTime, pNodeAnim);

			NodeTransformation = matTranslation * matRotation * matScale;
		}

		aiMatrix4x4 GlobalTransformation = parent * NodeTransformation;

		if (BoneMapping.find(NodeName) != BoneMapping.end()) {
			uint32_t BoneIndex = BoneMapping[NodeName];
			BoneInfos[BoneIndex].FinalTransformation = GlobalInverseTransform * GlobalTransformation * BoneInfos[BoneIndex].Offset;
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
			ReadNodeHierarchy(animTime, node->mChildren[i], GlobalTransformation);

	}

}