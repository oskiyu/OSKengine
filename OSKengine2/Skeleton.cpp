#include "Skeleton.h"

#include "Assert.h"
#include "AnimationSkin.h"
#include "AnimationExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void Skeleton::UpdateMatrices(const AnimationSkin& skin) {
	// Actualizar matrices de los huesos
	if (skin.rootIndex != std::numeric_limits<UIndex32>::max())
		nodes.at(skin.rootIndex).UpdateSkeletonTree(glm::mat4(1.0f), this);
	else
		nodes.at(GetRootNodeIndex()).UpdateSkeletonTree(glm::mat4(1.0f), this);
}

UIndex32 Skeleton::GetRootNodeIndex() const {
	for (auto& [index, node] : nodes)
		if (node.parentIndex == std::numeric_limits<UIndex32>::max())
			return index;

	return 0;
}

MeshNode& Skeleton::GetNode(UIndex32 nodeIndex) {
	OSK_ASSERT(nodes.contains(nodeIndex), BoneNotFoundException(nodeIndex));

	return nodes.at(nodeIndex);
}

Bone& Skeleton::GetBone(UIndex32 boneIndex, const AnimationSkin& skin) {
	const UIndex32 nodeIndex = skin.bonesIds.At(boneIndex);
	OSK_ASSERT(nodes.contains(nodeIndex), BoneNotFoundException(boneIndex));

	return nodes.at(nodeIndex);
}

MeshNode& Skeleton::GetNode(std::string_view name) {
	OSK_ASSERT(nodesByName.contains(name), BoneNotFoundException(name));

	return GetNode(nodesByName.find(name)->second);
}

Bone& Skeleton::GetBone(std::string_view name, const AnimationSkin& skin) {
	OSK_ASSERT(nodesByName.contains(name), BoneNotFoundException(name));

	return GetBone(nodesByName.find(name)->second, skin);
}

void Skeleton::_AddNode(const MeshNode& node) {
	const UIndex32 nodeIndex = node.thisIndex;

	OSK_ASSERT(!nodes.contains(nodeIndex), BoneAlreadyAddedException(node.thisIndex));

	nodes[nodeIndex] = node;
	nodesByName[node.name] = nodeIndex;
}
