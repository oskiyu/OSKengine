#include "Skeleton.h"

#include "Assert.h"
#include "AnimationSkin.h"
#include "AnimationExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void Skeleton::UpdateMatrices(const AnimationSkin& skin) {
	// Actualizar matrices de los huesos
	if (skin.rootIndex != std::numeric_limits<UIndex32>::max())
		nodes.Get(skin.rootIndex).UpdateSkeletonTree(glm::mat4(1.0f), *this);
	else
		nodes.Get(GetRootNodeIndex()).UpdateSkeletonTree(glm::mat4(1.0f), *this);
}

UIndex32 Skeleton::GetRootNodeIndex() const {
	for (auto& [index, node] : nodes)
		if (node.parentIndex == std::numeric_limits<UIndex32>::max())
			return index;

	return 0;
}

MeshNode* Skeleton::GetNode(UIndex32 nodeIndex) const {
	OSK_ASSERT(nodes.ContainsKey(nodeIndex), BoneNotFoundException(nodeIndex));

	return &nodes.Get(nodeIndex);
}

Bone* Skeleton::GetBone(UIndex32 boneIndex, const AnimationSkin& skin) const {
	const UIndex32 nodeIndex = skin.bonesIds.At(boneIndex);
	OSK_ASSERT(nodes.ContainsKey(nodeIndex), BoneNotFoundException(boneIndex));

	return &nodes.Get(nodeIndex);
}

MeshNode* Skeleton::GetNode(std::string_view name) const {
	OSK_ASSERT(nodesByName.ContainsKey((std::string)name), BoneNotFoundException(name));

	return GetNode(nodesByName.Get((std::string)name));
}

Bone* Skeleton::GetBone(std::string_view name, const AnimationSkin& skin) const {
	OSK_ASSERT(nodesByName.ContainsKey((std::string)name), BoneNotFoundException(name));

	return GetBone(nodesByName.Get((std::string)name), skin);
}

void Skeleton::_AddNode(const MeshNode& node) {
	const UIndex32 nodeIndex = node.thisIndex;

	OSK_ASSERT(!nodes.ContainsKey(nodeIndex), BoneAlreadyAddedException(node.thisIndex));

	nodes.Insert(nodeIndex, node);
	nodesByName.Insert(node.name, nodeIndex);
}
