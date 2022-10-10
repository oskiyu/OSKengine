#include "Skeleton.h"

#include "AnimationSkin.h"

using namespace OSK;
using namespace OSK::GRAPHICS;


void Skeleton::UpdateMatrices(const AnimationSkin& skin) {
	// Actualizar matrices de los huesos
	if (skin.rootIndex != std::numeric_limits<TIndex>::max())
		nodes.Get(skin.rootIndex).UpdateSkeletonTree(glm::mat4(1.0f), *this);
	else
		nodes.Get(GetRootNodeIndex()).UpdateSkeletonTree(glm::mat4(1.0f), *this);
}

TIndex Skeleton::GetRootNodeIndex() const {
	for (auto& [index, node] : nodes)
		if (node.parentIndex == std::numeric_limits<TIndex>::max())
			return index;

	return 0;
}

MeshNode* Skeleton::GetNode(TIndex nodeIndex) const {
	OSK_ASSERT(nodes.ContainsKey(nodeIndex),
		"Se ha intentado acceder al bone "
		+ std::to_string(nodeIndex) + " pero no existe.");

	return &nodes.Get(nodeIndex);
}

Bone* Skeleton::GetBone(TIndex boneIndex, const AnimationSkin& skin) const {
	const TIndex nodeIndex = skin.bonesIds.At(boneIndex);
	OSK_ASSERT(nodes.ContainsKey(nodeIndex),
		"Se ha intentado acceder al bone "
		+ std::to_string(nodeIndex) + " pero no existe.");

	return &nodes.Get(nodeIndex);
}

MeshNode* Skeleton::GetNode(const std::string& name) const {
	OSK_ASSERT(nodesByName.ContainsKey(name),
		"Se ha intentado acceder al bone "
		+ name + " pero no existe.");

	return GetNode(nodesByName.Get(name));
}

Bone* Skeleton::GetBone(const std::string& name, const AnimationSkin& skin) const {
	OSK_ASSERT(nodesByName.ContainsKey(name),
		"Se ha intentado acceder al bone "
		+ name + " pero no existe.");

	return GetBone(nodesByName.Get(name), skin);
}

void Skeleton::_AddNode(const MeshNode& node) {
	const TIndex nodeIndex = node.thisIndex;

	OSK_ASSERT(!nodes.ContainsKey(nodeIndex),
		"Se ha añadir el bone "
		+ std::to_string(nodeIndex) + " pero ya existe.");

	nodes.Insert(nodeIndex, node);
	nodesByName.Insert(node.name, nodeIndex);
}
