#include "Animator.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "Math.h"

#include "AnimationExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void Animator::Setup(const glm::mat4& initialTransform) {
	m_initialTransform = initialTransform;
	m_inverseInitialTransform = glm::inverse(m_initialTransform);
}

void Animator::Update(TDeltaTime deltaTime) {
	if (m_activeAnimations.empty() || !m_activeSkinIndex.has_value()) {
		return;
	}

	for (const UIndex64 index : m_activeAnimations) {
		m_animations[index].Update(deltaTime, *GetActiveSkin());
	}
	
	for (auto& matrix : m_boneMatrices) {
		matrix = glm::mat4(1.0f);
	}

	for (const UIndex64 animationIndex : m_activeAnimations) {
		const Animation& animation = m_animations[animationIndex];

		for (UIndex32 boneIndex = 0; boneIndex < GetActiveSkin()->bonesIds.GetSize(); boneIndex++) {
			const float ratio = 1.0f;// / activeAnimations.GetSize();

			// Transforma de espacio local a espacio de joint.
			// Contiene la inversa del transform original del hueso.
			const glm::mat4 inverseMatrix = GetActiveSkin()->inverseMatrices[boneIndex] * m_inverseInitialTransform;
			const glm::mat4 boneMatrix = animation.GetSkeleton().GetBone(boneIndex, *GetActiveSkin()).globalMatrix;

			m_boneMatrices[boneIndex] *= ratio * boneMatrix * inverseMatrix;
		}
	}

	for (auto& matrix : m_boneMatrices) matrix = m_initialTransform * matrix;
}

void Animator::AddActiveAnimation(std::string_view name) {
	OSK_ASSERT(m_animationsByName.contains(name), ModelAnimationNotFoundException(name));
	m_activeAnimations.insert(m_animationsByName.find(name)->second);
}

void Animator::RemoveActiveAnimation(std::string_view name) {
	OSK_ASSERT(m_animationsByName.contains(name), ModelAnimationNotFoundException(name));
	m_activeAnimations.erase(m_animationsByName.find(name)->second);
}

void Animator::AddAnimation(const Animation& animation) {
	m_animations.Insert(animation);
	m_animationsByName[static_cast<std::string>(animation.GetName())] = m_animations.GetSize() - 1;

	for (UIndex64 i = m_boneMatrices.GetSize(); i < animation.GetSkeleton().GetBoneCount(); i++) {
		m_boneMatrices.Insert(glm::mat4(1.0f));
	}
}

void Animator::AddSkin(const AnimationSkin& skin) {
	m_animationSkins.Insert(skin);
	m_skinsByName[static_cast<std::string>(skin.name)] = m_animationSkins.GetSize() - 1;
}

void Animator::SetActiveSkin(std::string_view name) {
	m_activeSkinIndex = m_skinsByName.find(name)->second;
}

const AnimationSkin* Animator::GetActiveSkin() const {
	return m_activeSkinIndex.has_value()
		? &m_animationSkins.At(m_activeSkinIndex.value())
		: nullptr;
}

const AnimationSkin& Animator::GetSkin(UIndex32 index) const {
	OSK_ASSERT(index < m_animationSkins.GetSize(), ModelSkinNotFoundException(std::format("índice {}", index)));
	return m_animationSkins[index];
}

glm::mat4 Animator::GetInitialTransform() const {
	return m_initialTransform;
}

const DynamicArray<glm::mat4>& Animator::GetFinalSkeletonMatrices() const {
	return m_boneMatrices;
}

bool Animator::HasAnimations() const {
	return !m_animations.IsEmpty();
}
