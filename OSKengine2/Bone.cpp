#include "Bone.h"

#include "Animator.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void MeshNode::UpdateSkeletonTree(const glm::mat4& prevMatrix, const Animator& animator) {
	globalMatrix = prevMatrix * GetLocalMatrix();

	for (const TIndex child : childIndices)
		animator.GetNode(child)->UpdateSkeletonTree(originalMatrix, animator);
}
