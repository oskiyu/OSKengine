#include "Bone.h"

#include "Animator.h"
#include "Skeleton.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void MeshNode::UpdateSkeletonTree(const glm::mat4& prevMatrix, const Skeleton& skeleton) {
	globalMatrix = prevMatrix * GetLocalMatrix();
	// globalMatrix = originalMatrix * GetLocalMatrix();

	for (const TIndex child : childIndices)
		skeleton.GetNode(child)->UpdateSkeletonTree(globalMatrix, skeleton);
}
