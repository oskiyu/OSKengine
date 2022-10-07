#include "Animator.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "Material.h"
#include "Math.h"

using namespace OSK;
using namespace OSK::GRAPHICS;

void Animator::Setup() {
	materialInstance = Engine::GetRenderer()->GetMaterialSystem()->LoadMaterial("Resources/PbrMaterials/Animated/material_pbr.json")->CreateInstance().GetPointer();

	for (TSize i = 0; i < _countof(boneBuffers); i++) {
		boneBuffers[i] = Engine::GetRenderer()->GetMemoryAllocator()->CreateStorageBuffer(sizeof(glm::mat4) * boneMatrices.GetSize()).GetPointer();

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

void Animator::Update(TDeltaTime deltaTime, const Vector3f& globalScale) {
	if (activeAnimation != "") {
		availableAnimations.Get(activeAnimation).Update(deltaTime, *this);

		for (auto& [index, node] : nodes)
			if (node.parentIndex == std::numeric_limits<TIndex>::max())
				node.UpdateSkeletonTree(glm::scale(glm::mat4(1.0f), globalScale.ToGLM()), *this);
	}

	for (TIndex boneIndex = 0; boneIndex < GetActiveSkin()->bonesIds.GetSize(); boneIndex++)
		boneMatrices[boneIndex] = GetBone(boneIndex)->globalMatrix * GetActiveSkin()->inverseMatrices[boneIndex];

	boneBuffers[Engine::GetRenderer()->GetCurrentCommandListIndex()]->MapMemory();
	boneBuffers[Engine::GetRenderer()->GetCurrentCommandListIndex()]->Write(boneMatrices.GetData(), sizeof(glm::mat4) * boneMatrices.GetSize());
	boneBuffers[Engine::GetRenderer()->GetCurrentCommandListIndex()]->Unmap();
}

void Animator::DeactivateAnimation() {
	activeAnimation = "";
}

void Animator::SetActiveAnimation(const std::string& name) {
	OSK_ASSERT(availableAnimations.ContainsKey(name),
		"Se ha intentado acceder a la animación "
		+ name + " pero no existe.");

	activeAnimation = name;
}

void Animator::_AddNode(const MeshNode& bone) {
	OSK_ASSERT(!nodes.ContainsKey(bone.thisIndex),
		"Se ha añadir el bone "
		+ std::to_string(bone.thisIndex) + " pero ya existe.");

	nodes.Insert(bone.thisIndex, bone);
	boneMatrices.Insert(glm::mat4(1.0f));
}

MeshNode* Animator::GetNode(TIndex nodeIndex) const {
	OSK_ASSERT(nodes.ContainsKey(nodeIndex),
		"Se ha intentado acceder al bone "
		+ std::to_string(nodeIndex) + " pero no existe.");

	return &nodes.Get(nodeIndex);
}

Bone* Animator::GetBone(TIndex boneIndex) const {
	if (GetActiveSkin() == nullptr)
		return nullptr;

	const TIndex nodeIndex = GetActiveSkin()->bonesIds.At(boneIndex);
	OSK_ASSERT(nodes.ContainsKey(nodeIndex),
		"Se ha intentado acceder al bone "
		+ std::to_string(nodeIndex) + " pero no existe.");

	return &nodes.Get(nodeIndex);
}

void Animator::_AddAnimation(const Animation& animation) {
	OSK_ASSERT(!availableAnimations.ContainsKey(animation.name),
		"Se ha intentado añadir a la animación "
		+ animation.name + " pero ya existe.");

	availableAnimations.Insert(animation.name, animation);
}

void Animator::_AddSkin(AnimationSkin&& skin) {
	OSK_ASSERT(!availableSkins.ContainsKey(skin.name),
		"Se ha intentado añadir la skin "
		+ skin.name + " pero ya existe.");

	availableSkins.InsertMove(skin.name, std::move(skin));
}

void Animator::SetActiveSkin(const std::string& name) {
	OSK_ASSERT(availableSkins.ContainsKey(name),
		"Se ha intentado acceder a la skin "
		+ name + " pero no existe.");

	activeSkin = name;
}

const AnimationSkin* Animator::GetActiveSkin() const {
	return activeSkin == "" ? nullptr : &availableSkins.Get(activeSkin);
}

const MaterialInstance* Animator::GetMaterialInstance() const {
	return materialInstance.GetPointer();
}
