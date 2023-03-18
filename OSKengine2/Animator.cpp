#include "Animator.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "Math.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void Animator::Setup(const glm::mat4& initialTransform) {
	this->initialTransform = initialTransform;

	materialInstance = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/Materials/PBR/animated_direct_pbr.json")->CreateInstance().GetPointer();

	for (TSize i = 0; i < _countof(boneBuffers); i++) {
		boneBuffers[i] = Engine::GetRenderer()->GetAllocator()->CreateStorageBuffer(sizeof(glm::mat4) * boneMatrices.GetSize()).GetPointer();

		boneBuffers[i]->MapMemory();
		boneBuffers[i]->Write(boneMatrices.GetData(), boneMatrices.GetSize() * sizeof(glm::mat4));
		boneBuffers[i]->Unmap();
	}

	const GpuDataBuffer* buffers[3] = {
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

		for (TIndex boneIndex = 0; boneIndex < GetActiveSkin()->bonesIds.GetSize(); boneIndex++) {
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
	OSK_ASSERT(availableAnimations.ContainsKey(name),
		"Se ha intentado acceder a la animación "
		+ name + " pero no existe.");

	activeAnimations.Insert(name);
}

void Animator::RemoveActiveAnimation(const std::string& name) {
	activeAnimations.Remove(name);
}

void Animator::_AddAnimation(const Animation& animation) {
	OSK_ASSERT(!availableAnimations.ContainsKey(animation.name),
		"Se ha intentado añadir a la animación "
		+ animation.name + " pero ya existe.");

	availableAnimations.Insert(animation.name, animation);

	for (const auto& node : nodes)
		availableAnimations.Get(animation.name).skeleton._AddNode(node);
}

void Animator::_AddNode(const MeshNode& node) {
	nodes.Insert(node);
	boneMatrices.Insert(glm::mat4(1.0f));
}

void Animator::_AddSkin(AnimationSkin&& skin) {
	const TIndex skinIndex = availableSkins.GetSize();

	availableSkins.Insert(skin);
	availableSkinsByName.Insert(skin.name, skinIndex);
}

void Animator::SetActiveSkin(const std::string& name) {
	OSK_ASSERT(availableSkinsByName.ContainsKey(name),
		"Se ha intentado acceder a la skin "
		+ name + " pero no existe.");

	activeSkin = name;
}

const AnimationSkin* Animator::GetActiveSkin() const {
	return activeSkin == "" ? nullptr : &availableSkins.At(availableSkinsByName.Get(activeSkin));
}

const AnimationSkin& Animator::GetSkin(TIndex index) const {
	OSK_ASSERT(index < availableSkins.GetSize(), "No existe la skin con el índice " + std::to_string(index));

	return availableSkins[index];
}

const MaterialInstance* Animator::GetMaterialInstance() const {
	return materialInstance.GetPointer();
}

glm::mat4 Animator::GetInitialTransform() const {
	return initialTransform;
}
