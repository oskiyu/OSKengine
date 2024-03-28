#include "Animation.h"

#include "Math.h"
#include "Animator.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

Animation::Animation(
	const std::string& name, 
	const DynamicArray<AnimationSampler>& samplers, 
	const DynamicArray<AnimationChannel>& channels,
	const DynamicArray<AnimationBone>& bones) : m_name(name), m_samplers(samplers), m_channels(channels), m_skeleton(bones)
{
	for (const auto& sampler : m_samplers) {
		for (const TDeltaTime timestamp : sampler.timestamps) {
			m_startTime = glm::min(m_startTime, timestamp);
			m_endTime = glm::max(m_endTime, timestamp);
		}
	}
}

void Animation::Update(TDeltaTime deltaTime, const AnimationSkin& skin) {
	m_currentTime += deltaTime;

	if (m_currentTime > m_endTime) {
		if (m_shouldLoop)
			m_currentTime -= m_endTime;
		else
			return;
	}

	for (const auto& channel : m_channels) {
		const AnimationSampler& sampler = m_samplers[channel.samplerIndex];

		for (UIndex64 timestampIndex = 0; timestampIndex < sampler.timestamps.GetSize() - 1; timestampIndex++) {
			// TODO: check interpolation type

			const auto timestampA = sampler.timestamps[timestampIndex];
			const auto timestampB = sampler.timestamps[timestampIndex + 1];

			const bool isCurrentTimeBetweenStamps =
				m_currentTime >= timestampA &&
				m_currentTime <= timestampB;

			if (!isCurrentTimeBetweenStamps)
				return;

			// Si el timestamp actual está entre los dos, se hace una interpolación lineal.

			const float ratio = (m_currentTime - timestampA) / (timestampB - timestampA);

			AnimationBone& bone = m_skeleton.GetBone(channel.nodeId);

			const auto& outputA = sampler.outputs[timestampIndex];
			const auto& outputB = sampler.outputs[timestampIndex + 1];

			switch (channel.type) {

				case AnimationChannel::ChannelType::TRANSLATION: {
					const Vector4f vec4 = MATH::LinearInterpolation_Fast(outputA, outputB, ratio);
					bone.position = { vec4.x, vec4.y, vec4.Z };
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

					bone.rotation = Quaternion::FromGlm(glm::normalize(glm::slerp(prev, next, ratio)));
				}
				break;

				case AnimationChannel::ChannelType::SCALE: {
					const Vector4f vec4 = MATH::LinearInterpolation_Fast(outputA, outputB, ratio);
					bone.scale = { vec4.x, vec4.y, vec4.Z };
				}
				break;

			}
		}
	}

	m_skeleton.UpdateMatrices(skin);
}

void Animation::SetLooping(bool isLooping) {
	m_shouldLoop = isLooping;
}

std::string_view Animation::GetName() const {
	return m_name;
}

const Skeleton& Animation::GetSkeleton() const {
	return m_skeleton;
}
