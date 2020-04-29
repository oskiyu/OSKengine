#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Animation.h"
#include "Bone.h"

#include <vector>
#include <map>

namespace OSK {

	constexpr uint32_t MAX_BONES_AMOUNT = 32;

	class Skeleton {

	public:

		Skeleton();


		Skeleton(const std::vector<Bone>& bones);


		~Skeleton();


		Bone* GetBone(const std::string& name);


		void PlayAnimation(Animation* animation, const bool& reset = true);


		void StopAnimation();


		void Update(const deltaTime_t& deltaTime);


		std::map<std::string, uint32_t> BoneMapping;


		std::vector<Bone> Bones;


		uint32_t NumberOfBones = 0;

	private:

		float_t time = 0.0f;


		OSK::Animation* activeAnimation = nullptr;

	};

}