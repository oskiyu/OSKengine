#include "ModelComponent3D.h"

#include "MaterialInstance.h"
#include "Model3D.h"
#include "Mesh3D.h"
#include "Material.h"
#include "IMaterialSlot.h"

using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void ModelComponent3D::SetModel(Model3D* model) {
	this->model = model;

	USize32 prevSize = static_cast<USize32>(meshesMaterialInstances.GetSize());
	meshesMaterialInstances.Resize(model->GetMeshes().GetSize(), nullptr);

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();

	if (material != nullptr) {
		for (UIndex32 i = prevSize; i < meshesMaterialInstances.GetSize(); i++) {
			meshesMaterialInstances.At(i) = material->CreateInstance().GetPointer();

			for (const auto& [slotName, bindings] : texturesBound)
				for (const auto& [bindingName, binding] : bindings)
					meshesMaterialInstances.At(i)->GetSlot(slotName)->SetTexture(bindingName, binding);

			for (const auto& [slotName, bindings] : imagesBound)
				for (const auto& [bindingName, binding] : bindings)
					meshesMaterialInstances.At(i)->GetSlot(slotName)->SetGpuImage(bindingName, binding);

			for (const auto& [slotName, bindings] : uniformBuffersBound)
				for (const auto& [bindingName, binding] : bindings)
					meshesMaterialInstances.At(i)->GetSlot(slotName)->SetUniformBuffer(bindingName, *binding);
		}
	}
}

void ModelComponent3D::SetMaterial(Material* material) {
	this->material = material;

	const GpuImageViewConfig viewConfig = GpuImageViewConfig::CreateSampled_Default();

	for (auto& i : meshesMaterialInstances) {
		if (i.GetPointer() == nullptr) {
			i = material->CreateInstance().GetPointer();

			for (const auto& [slotName, bindings] : texturesBound)
				for (const auto& [bindingName, binding] : bindings)
					i->GetSlot(slotName)->SetTexture(bindingName, binding);

			for (const auto& [slotName, bindings] : imagesBound)
				for (const auto& [bindingName, binding] : bindings)
					i->GetSlot(slotName)->SetGpuImage(bindingName, binding);

			for (const auto& [slotName, bindings] : uniformBuffersBound)
				for (const auto& [bindingName, binding] : bindings)
					i->GetSlot(slotName)->SetUniformBuffer(bindingName, *binding);
		}
	}
}

Model3D* ModelComponent3D::GetModel() const {
	return model;
}

MaterialInstance* ModelComponent3D::GetMeshMaterialInstance(UIndex32 meshId) const {
	return meshesMaterialInstances.At(meshId).GetPointer();
}

void ModelComponent3D::BindTextureForAllMeshes(const std::string& slot, const std::string& binding, const ASSETS::Texture* texture) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetTexture(binding, texture);
		i->GetSlot(slot)->FlushUpdate();
	}

	if (!texturesBound.contains(slot))
		texturesBound[slot] = {};

	texturesBound.at(slot)[binding] = texture;
}

void ModelComponent3D::BindGpuImageForAllMeshes(const std::string& slot, const std::string& binding, const IGpuImageView* image) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetGpuImage(binding, image);
		i->GetSlot(slot)->FlushUpdate();
	}

	if (!imagesBound.contains(slot))
		imagesBound[slot] = {};

	imagesBound.at(slot)[binding] = image;
}

void ModelComponent3D::BindUniformBufferForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::GpuBuffer* buffer) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetUniformBuffer(binding, *buffer);
		i->GetSlot(slot)->FlushUpdate();
	}


	if (!uniformBuffersBound.contains(slot))
		uniformBuffersBound[slot] = {};

	uniformBuffersBound.at(slot)[binding] = buffer;
}

/*template <> struct std::hash<OSK::Pair<std::string, std::string>> {
	size_t operator()(const Pair<std::string, std::string>& x) const
	{
		return std::hash<std::string>()(x.first);
	}
};*/
