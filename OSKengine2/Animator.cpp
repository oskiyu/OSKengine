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

	m_materialInstance = Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/PBR/animated_direct_pbr.json")->CreateInstance().GetPointer();

	for (auto& buffer : m_boneBuffers) {
		buffer = Engine::GetRenderer()->GetAllocator()->CreateStorageBuffer(sizeof(glm::mat4) * m_boneMatrices.GetSize()).GetPointer();

		buffer->MapMemory();
		buffer->Write(m_boneMatrices.GetData(), m_boneMatrices.GetSize() * sizeof(glm::mat4));
		buffer->Unmap();
	}

	const GpuBuffer* buffers[3] = {
		m_boneBuffers[0].GetPointer(),
		m_boneBuffers[1].GetPointer(),
		m_boneBuffers[2].GetPointer()
	};

	m_materialInstance->GetSlot("animation")->SetStorageBuffers("animation", buffers);
	m_materialInstance->GetSlot("animation")->FlushUpdate();
}

void Animator::Update(TDeltaTime deltaTime) {
	for (const std::string& animationName : m_activeAnimations)
		m_availableAnimations.at(animationName).Update(deltaTime, *GetActiveSkin());
	
	for (auto& matrix : m_boneMatrices)
		matrix = glm::mat4(1.0f);

	for (const std::string& animationName : m_activeAnimations) {
		const Animation& animation = m_availableAnimations.at(animationName);

		for (UIndex32 boneIndex = 0; boneIndex < GetActiveSkin()->bonesIds.GetSize(); boneIndex++) {
			const float ratio = 1.0f;// / activeAnimations.GetSize();

			// Transforma de espacio local a espacio de joint.
			// Contiene la inversa del transform original del hueso.
			const glm::mat4 inverseMatrix = GetActiveSkin()->inverseMatrices[boneIndex];
			const glm::mat4 boneMatrix = animation.skeleton.GetBone(boneIndex, *GetActiveSkin()).globalMatrix;
			
			m_boneMatrices[boneIndex] *= ratio * boneMatrix * inverseMatrix;
		}
	}

	for (auto& matrix : m_boneMatrices) matrix = m_initialTransform * matrix;

	m_boneBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->MapMemory();
	m_boneBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->Write(m_boneMatrices.GetData(), sizeof(glm::mat4) * m_boneMatrices.GetSize());
	m_boneBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->Unmap();
}

void Animator::AddActiveAnimation(std::string_view name) {
	OSK_ASSERT(m_availableAnimations.contains(name), ModelAnimationNotFoundException(name));

	m_activeAnimations.Insert(static_cast<std::string>(name));
}

void Animator::RemoveActiveAnimation(std::string_view name) {
	m_activeAnimations.Remove(static_cast<std::string>(name));
}

void Animator::_AddAnimation(const Animation& animation) {
	m_availableAnimations[animation.name] = animation;

	for (const auto& node : m_nodes)
		m_availableAnimations.at(animation.name).skeleton._AddNode(node);
}

void Animator::_AddNode(const MeshNode& node) {
	m_nodes.Insert(node);
	m_boneMatrices.Insert(glm::mat4(1.0f));
}

void Animator::_AddSkin(const AnimationSkin& skin) {
	const UIndex64 skinIndex = m_availableSkins.GetSize();

	m_availableSkins.Insert(skin);
	m_availableSkinsByName[skin.name] = skinIndex;
}

void Animator::SetActiveSkin(std::string_view name) {
	OSK_ASSERT(m_availableSkinsByName.contains(name), ModelSkinNotFoundException(name));

	m_activeSkin = name;
}

const AnimationSkin* Animator::GetActiveSkin() const {
	return m_activeSkin == ""
		? nullptr 
		: &m_availableSkins.At(m_availableSkinsByName.at(m_activeSkin));
}

const AnimationSkin& Animator::GetSkin(UIndex32 index) const {
	OSK_ASSERT(index < m_availableSkins.GetSize(), ModelSkinNotFoundException(std::format("índice {}", index)));

	return m_availableSkins[index];
}

const MaterialInstance* Animator::GetMaterialInstance() const {
	return m_materialInstance.GetPointer();
}

glm::mat4 Animator::GetInitialTransform() const {
	return m_initialTransform;
}
