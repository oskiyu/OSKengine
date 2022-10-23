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

			for (auto& t : texturesBound)
				meshesMaterialInstances.At(i)->GetSlot(t.first.first)->SetTexture(t.first.second, t.second);

			for (auto& t : imagesBound)
				meshesMaterialInstances.At(i)->GetSlot(t.first.first)->SetGpuImage(t.first.second, t.second);

			for (auto& t : uniformBuffersBound)
				meshesMaterialInstances.At(i)->GetSlot(t.first.first)->SetUniformBuffer(t.first.second, t.second);
		}
	}
}

void ModelComponent3D::SetMaterial(Material* material) {
	this->material = material;

	for (auto& i : meshesMaterialInstances) {
		if (i.GetPointer() == nullptr) {
			i = material->CreateInstance().GetPointer();

			for (auto& t : texturesBound)
				i->GetSlot(t.first.first)->SetTexture(t.first.second, t.second);

			for (auto& t : imagesBound)
				i->GetSlot(t.first.first)->SetGpuImage(t.first.second, t.second);

			for (auto& t : uniformBuffersBound)
				i->GetSlot(t.first.first)->SetUniformBuffer(t.first.second, t.second);
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

	texturesBound.Insert({ slot, binding }, texture);
}

void ModelComponent3D::BindGpuImageForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::GpuImage* image) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetGpuImage(binding, image);
		i->GetSlot(slot)->FlushUpdate();
	}

	imagesBound.Insert({ slot, binding }, image);
}

void ModelComponent3D::BindUniformBufferForAllMeshes(const std::string& slot, const std::string& binding, const GRAPHICS::IGpuUniformBuffer* buffer) {
	for (auto& i : meshesMaterialInstances) {
		i->GetSlot(slot)->SetUniformBuffer(binding, buffer);
		i->GetSlot(slot)->FlushUpdate();
	}

	uniformBuffersBound.Insert({ slot, binding }, buffer);
}

template <> struct std::hash<OSK::Pair<std::string, std::string>> {
	size_t operator()(const Pair<std::string, std::string>& x) const
	{
		return std::hash<std::string>()(x.first);
	}
};
