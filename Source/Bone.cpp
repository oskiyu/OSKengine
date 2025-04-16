#include "Bone.h"

#include "Animator.h"
#include "Skeleton.h"

#include "OSKengine.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void AnimationBone::UpdateSkeletonTree(const glm::mat4& prevMatrix, Skeleton* skeleton) {
	globalMatrix = prevMatrix * GetLocalMatrix();

	for (const UIndex32 child : childIndices) {
		skeleton->GetBone(child).UpdateSkeletonTree(globalMatrix, skeleton);
	}
}
