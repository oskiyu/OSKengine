#pragma once

#include "OSKmacros.h"

#include "DynamicArray.hpp"

#include "Bone.h"
#include "AnimationChannel.h"
#include "AnimationSampler.h"
#include "Skeleton.h"

#include <limits>

namespace OSK::GRAPHICS {

	class Animator;
	struct AnimationSkin;

	/// <summary>
	/// Animación para un modelo 3D.
	/// </summary>
	class OSKAPI_CALL Animation {

	public:

		/// <summary> Mueve la animación. </summary>
		void Update(TDeltaTime deltaTime, const AnimationSkin& skin);

		/// <summary> Nombre original de la animación. </summary>
		std::string name;

		Skeleton skeleton{};

		DynamicArray<AnimationSampler> samplers;
		DynamicArray<AnimationChannel> channels;

		TDeltaTime startTime = 0.0f;
		TDeltaTime endTime = 0.0f;

		TDeltaTime currentTime = 0.0f;

		bool shouldLoop = true;

	};

}
