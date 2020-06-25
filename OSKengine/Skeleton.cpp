#include "Skeleton.h"

namespace OSK {

	Skeleton::Skeleton() {

	}


	Skeleton::Skeleton(const std::vector<Bone>& bones) {
		Bones = bones;
	}


	Skeleton::~Skeleton() {
		delete activeAnimation;
	}


	void Skeleton::PlayAnimation(Animation* animation, const bool& reset) {
		if (activeAnimation) {
			if (animation->Priority >= activeAnimation->Priority) {
				activeAnimation = animation;
			}
		}
		else {
			activeAnimation = animation;
		}

		if (reset) {
			time = activeAnimation->StartTime;
		}
	}


	/*Bone* Skeleton::GetBone(const std::string& name) {
		for (auto i : Bones) {
			if (i.Name == name)
				return &i;
		}

		return nullptr;
	}*/


	void Skeleton::StopAnimation() {
		time = 0.0f;
		activeAnimation = nullptr;
	}


	void Skeleton::Update(const deltaTime_t& deltaTime) {
		if (!activeAnimation || Bones.empty()) {
			return;
		}

		time += activeAnimation->Speed * deltaTime * 60;

		if (time < activeAnimation->StartTime) {
			time = activeAnimation->StartTime;
		}

		if (time > activeAnimation->EndTime) {
			if (activeAnimation->Loop) {
				time = activeAnimation->StartTime;
			}
			else {
				StopAnimation();
			}
		}

		for (auto &b : Bones) {
			b.UpdateTransform(time);
			b.Transform.UpdateModel();
		}
	}


	Bone Skeleton::GetBoneByID(const uint32_t& ID) const {
		for (auto b : Bones)
			if (b.ID == ID)
				return b;

		return Bones[0];
	}

}