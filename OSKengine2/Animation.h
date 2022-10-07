#pragma once

#include "OSKmacros.h"

#include "DynamicArray.hpp"

#include "Bone.h"
#include "AnimationChannel.h"
#include "AnimationSampler.h"

#include <limits>

namespace OSK::GRAPHICS {

	class Animator;

	/// <summary>
	/// Animaci�n para un modelo 3D.
	/// </summary>
	class OSKAPI_CALL Animation {

	public:

		/// <summary> Mueve la animaci�n. </summary>
		void Update(TDeltaTime deltaTime, const Animator& owner);

		/// <summary> Nombre original de la animaci�n. </summary>
		std::string name;

		DynamicArray<AnimationSampler> samplers;
		DynamicArray<AnimationChannel> channels;

		TDeltaTime startTime = 0.0f;
		TDeltaTime endTime = 0.0f;

		TDeltaTime currentTime = 0.0f;

		bool shouldLoop = true;

	};

}
