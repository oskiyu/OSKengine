#include "Animation.h"

#include "Math.h"
#include "Animator.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void Animation::Update(TDeltaTime deltaTime, const Animator& owner) {
	currentTime += deltaTime;

	if (currentTime > endTime) {
		if (shouldLoop)
			currentTime -= endTime;
		else
			return;
	}

	for (auto& channel : channels) {
		const AnimationSampler& sampler = samplers[channel.samplerIndex];

		for (TSize i = 0; i < sampler.timestamps.GetSize() - 1; i++) {
			// TODO: check interpolation type

			// Si el timestamp actual está entre los dos, se hace una interpolación lineal.
			if (currentTime >= sampler.timestamps[i] && currentTime <= sampler.timestamps[i + 1]) {
				const float ratio = (currentTime - sampler.timestamps[i]) / (sampler.timestamps[i + 1] - sampler.timestamps[i]);

				MeshNode* node = owner.GetNode(channel.nodeId);

				switch (channel.type) {

				case AnimationChannel::ChannelType::TRANSLATION: {
					const Vector4f vec4 = MATH::LinearInterpolation_Fast(sampler.outputs[i], sampler.outputs[i + 1], ratio);
					node->position = { vec4.X, vec4.Y, vec4.Z };
				}
					break;

				case AnimationChannel::ChannelType::ROTATION: {
					glm::quat prev{};
					prev.x = sampler.outputs[i].X;
					prev.y = sampler.outputs[i].Y;
					prev.z = sampler.outputs[i].Z;
					prev.w = sampler.outputs[i].W;

					glm::quat next{};
					next.x = sampler.outputs[i + 1].X;
					next.y = sampler.outputs[i + 1].Y;
					next.z = sampler.outputs[i + 1].Z;
					next.w = sampler.outputs[i + 1].W;

					node->rotation = Quaternion::FromGlm(glm::normalize(glm::slerp(prev, next, ratio)));
				}
					break;

				case AnimationChannel::ChannelType::SCALE: {
					const Vector4f vec4 = MATH::LinearInterpolation_Fast(sampler.outputs[i], sampler.outputs[i + 1], ratio);
					node->scale = { vec4.X, vec4.Y, vec4.Z };
				}
					break;

				}
			
			}
		}
	}
}
