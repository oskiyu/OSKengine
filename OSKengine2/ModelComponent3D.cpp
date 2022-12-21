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

	TSize prevSize = meshesMaterialInstances.GetSize();
	meshesMaterialInstances.Resize(model->GetMeshes().GetSize(), nullptr);

	if (material != nullptr) {
		for (TSize i = prevSize; i < meshesMaterialInstances.GetSize(); i++) {
			meshesMaterialInstances.At(i) = material->CreateInstance().GetPointer();

			for (auto& [slotName, bindings] : texturesBound)
				for (auto& [bindingName, binding] : bindings)
					meshesMaterialInstances.At(i)->GetSlot(slotName)->SetTexture(bindingName, binding);

			for (auto& [slotName, bindings] : imagesBound)
				for (auto& [bindingName, binding] : bindings)
					meshesMaterialInstances.At(i)->GetSlot(slotName)->SetGpuImage(bindingName, binding);

			for (auto& [slotName, bindings] : uniformBuffersBound)
				for (auto& [bindingName, binding] : bindings)
					meshesMaterialInstances.At(i)->GetSlot(slotName)->SetUniformBuffer(bindingName, binding);
		}
	}
}

void ModelComponent3D::SetMaterial(Material* material) {
	this->material = material;

	for (auto& i : meshesMaterialInstances) {
		if (i.GetPointer() == nullptr) {
			i = material->CreateInstance().GetPointer();

			for (auto& [slotName, bindings] : texturesBound)
				for (auto& [bindingName, binding] : bindings)
					i->GetSlot(slotName)->SetTexture(bindingName, binding);

			for (auto& [slotName, bindings] : imagesBound)
				for (auto& [bindingName, binding] : bindings)
					i->GetSlot(slotName)->SetGpuImage(bindingName, binding);

			for (auto& [slotName, bindings] : uniformBuffersBound)
				for (auto& [bindingName, binding] : bindings)
					i->GetSlot(slotName)->SetUniformBuffer(bindingName, binding);
		}
	}
}

Model3D* ModelComponent3D::GetModel() const {
	return model;
}

MaterialInstance* ModelComponent3D::GetMeshMaterialInstance(TSize meshId) const {
	return meshesMaterialInstances.At(meshId).GetPointer();
}

void ModelComponent3D::BindTextureForAllMeshes(const std::string& slot, const std::string& binding, const ASSETS::Texture* texture) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetTexture(binding, texture);
		i->GetSlot(slot)->FlushUpdate();
	}

	if (!texturesBound.HasValue(slot))
		texturesBound.Insert(slot, {});

	texturesBound.Get(slot).Insert(binding, texture);
}

void ModelComponent3D::BindGpuImageForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::GpuImage* image) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetGpuImage(binding, image);
		i->GetSlot(slot)->FlushUpdate();
	}

	if (!imagesBound.HasValue(slot))
		imagesBound.Insert(slot, {});

	imagesBound.Get(slot).Insert(binding, image);
}

void ModelComponent3D::BindUniformBufferForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::IGpuUniformBuffer* buffer) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetUniformBuffer(binding, buffer);
		i->GetSlot(slot)->FlushUpdate();
	}


	if (!uniformBuffersBound.HasValue(slot))
		uniformBuffersBound.Insert(slot, {});

	uniformBuffersBound.Get(slot).Insert(binding, buffer);
}

/*template <> struct std::hash<OSK::Pair<std::string, std::string>> {
	size_t operator()(const Pair<std::string, std::string>& x) const
	{
		return std::hash<std::string>()(x.first);
	}
};*/
