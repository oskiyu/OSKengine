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

	constexpr uint32_t MAX_BONES_AMOUNT = 100;

	class OSKAPI_CALL Skeleton {

	public:

		Skeleton();


		Skeleton(const std::vector<Bone>& bones);


		~Skeleton();


		Bone* GetBone(const std::string& name);


		void PlayAnimation(Animation* animation, const bool& reset = true);


		void StopAnimation();


		void Update(const deltaTime_t& deltaTime);


		std::vector<Bone> Bones;


		std::map<std::string, uint32_t> BoneMap;


		Bone GetBoneByID(const uint32_t& ID) const;

	private:

		float_t time = 0.0f;


		OSK::Animation* activeAnimation = nullptr;

	};

}