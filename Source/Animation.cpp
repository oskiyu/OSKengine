#include "Animation.h"

#include "Math.h"
#include "Animator.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void Animation::Update(TDeltaTime deltaTime, const AnimationSkin& skin) {
	currentTime += deltaTime;

	if (currentTime > endTime) {
		if (shouldLoop)
			currentTime -= endTime;
		else
			return;
	}

	for (const auto& channel : channels) {
		const AnimationSampler& sampler = samplers[channel.samplerIndex];

		for (UIndex64 timestampIndex = 0; timestampIndex < sampler.timestamps.GetSize() - 1; timestampIndex++) {
			// TODO: check interpolation type

			const auto timestampA = sampler.timestamps[timestampIndex];
			const auto timestampB = sampler.timestamps[timestampIndex + 1];

			const bool isCurrentTimeBetweenStamps =
				currentTime >= timestampA &&
				currentTime <= timestampB;

			if (!isCurrentTimeBetweenStamps)
				return;

			// Si el timestamp actual está entre los dos, se hace una interpolación lineal.

			const float ratio = (currentTime - timestampA) / (timestampB - timestampA);

			MeshNode& node = skeleton.GetNode(channel.nodeId);

			const auto& outputA = sampler.outputs[timestampIndex];
			const auto& outputB = sampler.outputs[timestampIndex + 1];

			switch (channel.type) {

				case AnimationChannel::ChannelType::TRANSLATION: {
					const Vector4f vec4 = MATH::LinearInterpolation_Fast(outputA, outputB, ratio);
					node.position = { vec4.x, vec4.y, vec4.Z };
				}
				break;

				case AnimationChannel::ChannelType::ROTATION: {
					glm::quat prev{};
					prev.x = outputA.x;
					prev.y = outputA.y;
					prev.z = outputA.Z;
					prev.w = outputA.W;

					glm::quat next{};
					next.x = outputB.x;
					next.y = outputB.y;
					next.z = outputB.Z;
					next.w = outputB.W;

					node.rotation = Quaternion::FromGlm(glm::normalize(glm::slerp(prev, next, ratio)));
				}
				break;

				case AnimationChannel::ChannelType::SCALE: {
					const Vector4f vec4 = MATH::LinearInterpolation_Fast(outputA, outputB, ratio);
					node.scale = { vec4.x, vec4.y, vec4.Z };
				}
				break;

			}
		}
	}

	skeleton.UpdateMatrices(skin);
}
