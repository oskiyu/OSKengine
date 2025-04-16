#include "Skeleton.h"

#include "Assert.h"
#include "AnimationSkin.h"
#include "AnimationExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


Skeleton::Skeleton(const DynamicArray<AnimationBone>& bones) {
	for (const auto& bone : bones) {
		const UIndex32 boneIndex = bone.thisIndex;

		OSK_ASSERT(!m_bones.contains(boneIndex), BoneAlreadyAddedException(bone.thisIndex));

		m_bones[boneIndex] = bone;
		m_boneNameToIndex[bone.name] = boneIndex;
	}
}

void Skeleton::UpdateMatrices(const AnimationSkin& skin) {
	// Actualizar matrices de los huesos.
	const auto rootIndex = skin.rootIndex != std::numeric_limits<UIndex32>::max()
		? skin.rootIndex
		: GetRootNodeIndex();

	m_bones.at(rootIndex).UpdateSkeletonTree(glm::mat4(1.0f), this);
}

UIndex32 Skeleton::GetRootNodeIndex() const {
	for (auto& [index, node] : m_bones) {
		if (!node.parentIndex.has_value()) {
			return index;
		}
	}

	return 0;
}

void Skeleton::AddBone(const AnimationBone& bone) {
	m_bones[bone.thisIndex] = bone;
	m_boneNameToIndex[bone.name] = bone.thisIndex;
}

USize64 Skeleton::GetBoneCount() const {
	return m_bones.size();
}

AnimationBone& Skeleton::GetBone(UIndex32 nodeIndex) {
	OSK_ASSERT(m_bones.contains(nodeIndex), BoneNotFoundException(nodeIndex));
	return m_bones.at(nodeIndex);
}

const AnimationBone& Skeleton::GetBone(UIndex32 nodeIndex) const {
	OSK_ASSERT(m_bones.contains(nodeIndex), BoneNotFoundException(nodeIndex));
	return m_bones.at(nodeIndex);
}


AnimationBone& Skeleton::GetBone(UIndex32 boneIndex, const AnimationSkin& skin) {
	const UIndex32 nodeIndex = skin.bonesIds.At(boneIndex);
	OSK_ASSERT(m_bones.contains(nodeIndex), BoneNotFoundException(boneIndex));
	return m_bones.at(nodeIndex);
}

const AnimationBone& Skeleton::GetBone(UIndex32 boneIndex, const AnimationSkin& skin) const {
	const UIndex32 nodeIndex = skin.bonesIds.At(boneIndex);
	OSK_ASSERT(m_bones.contains(nodeIndex), BoneNotFoundException(boneIndex));
	return m_bones.at(nodeIndex);
}

AnimationBone& Skeleton::GetBone(std::string_view name) {
	OSK_ASSERT(m_boneNameToIndex.contains(name), BoneNotFoundException(name));

	return GetBone(m_boneNameToIndex.find(name)->second);
}

const AnimationBone& Skeleton::GetBone(std::string_view name) const {
	OSK_ASSERT(m_boneNameToIndex.contains(name), BoneNotFoundException(name));

	return GetBone(m_boneNameToIndex.find(name)->second);
}

AnimationBone& Skeleton::GetBone(std::string_view name, const AnimationSkin& skin) {
	OSK_ASSERT(m_boneNameToIndex.contains(name), BoneNotFoundException(name));

	return GetBone(m_boneNameToIndex.find(name)->second, skin);
}

const AnimationBone& Skeleton::GetBone(std::string_view name, const AnimationSkin& skin) const {
	OSK_ASSERT(m_boneNameToIndex.contains(name), BoneNotFoundException(name));

	return GetBone(m_boneNameToIndex.find(name)->second, skin);
}
