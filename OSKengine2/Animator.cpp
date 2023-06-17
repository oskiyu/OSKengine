#include "Animator.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "Math.h"

#include "AnimationExceptions.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void Animator::Setup(const glm::mat4& initialTransform) {
	this->initialTransform = initialTransform;

	materialInstance = Engine::GetRenderer()->GetMaterialSystem()
		->LoadMaterial("Resources/Materials/PBR/animated_direct_pbr.json")->CreateInstance().GetPointer();

	for (auto& buffer : boneBuffers) {
		buffer = Engine::GetRenderer()->GetAllocator()->CreateStorageBuffer(sizeof(glm::mat4) * boneMatrices.GetSize()).GetPointer();

		buffer->MapMemory();
		buffer->Write(boneMatrices.GetData(), boneMatrices.GetSize() * sizeof(glm::mat4));
		buffer->Unmap();
	}

	const GpuBuffer* buffers[3] = {
		boneBuffers[0].GetPointer(),
		boneBuffers[1].GetPointer(),
		boneBuffers[2].GetPointer()
	};

	materialInstance->GetSlot("animation")->SetStorageBuffers("animation", buffers);
	materialInstance->GetSlot("animation")->FlushUpdate();
}

void Animator::Update(TDeltaTime deltaTime) {
	for (const std::string& animationName : activeAnimations)
		availableAnimations.Get(animationName).Update(deltaTime, *GetActiveSkin());
	
	for (auto& matrix : boneMatrices)
		matrix = glm::mat4(1.0f);

	for (const std::string& animationName : activeAnimations) {
		const Animation& animation = availableAnimations.Get(animationName);

		for (UIndex32 boneIndex = 0; boneIndex < GetActiveSkin()->bonesIds.GetSize(); boneIndex++) {
			const float ratio = 1.0f;// / activeAnimations.GetSize();

			// Transforma de espacio local a espacio de joint.
			// Contiene la inversa del transform original del hueso.
			const glm::mat4 inverseMatrix = GetActiveSkin()->inverseMatrices[boneIndex];
			const glm::mat4 boneMatrix = animation.skeleton.GetBone(boneIndex, *GetActiveSkin())->globalMatrix;
			
			boneMatrices[boneIndex] *= ratio * boneMatrix * inverseMatrix;
		}
	}

	for (auto& matrix : boneMatrices) matrix = initialTransform * matrix;

	boneBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->MapMemory();
	boneBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->Write(boneMatrices.GetData(), sizeof(glm::mat4) * boneMatrices.GetSize());
	boneBuffers[Engine::GetRenderer()->GetCurrentResourceIndex()]->Unmap();
}

void Animator::AddActiveAnimation(const std::string& name) {
	OSK_ASSERT(availableAnimations.ContainsKey(name), ModelAnimationNotFoundException(name));

	activeAnimations.Insert(name);
}

void Animator::RemoveActiveAnimation(const std::string& name) {
	activeAnimations.Remove(name);
}

void Animator::_AddAnimation(const Animation& animation) {
	availableAnimations.Insert(animation.name, animation);

	for (const auto& node : nodes)
		availableAnimations.Get(animation.name).skeleton._AddNode(node);
}

void Animator::_AddNode(const MeshNode& node) {
	nodes.Insert(node);
	boneMatrices.Insert(glm::mat4(1.0f));
}

void Animator::_AddSkin(AnimationSkin&& skin) {
	const UIndex32 skinIndex = availableSkins.GetSize();

	availableSkins.Insert(skin);
	availableSkinsByName.Insert(skin.name, skinIndex);
}

void Animator::SetActiveSkin(const std::string& name) {
	OSK_ASSERT(availableSkinsByName.ContainsKey(name), ModelSkinNotFoundException(name));

	activeSkin = name;
}

const AnimationSkin* Animator::GetActiveSkin() const {
	return activeSkin == "" 
		? nullptr 
		: &availableSkins.At(availableSkinsByName.Get(activeSkin));
}

const AnimationSkin& Animator::GetSkin(UIndex32 index) const {
	OSK_ASSERT(index < availableSkins.GetSize(), ModelSkinNotFoundException(std::format("índice {}", index)));

	return availableSkins[index];
}

const MaterialInstance* Animator::GetMaterialInstance() const {
	return materialInstance.GetPointer();
}

glm::mat4 Animator::GetInitialTransform() const {
	return initialTransform;
}
