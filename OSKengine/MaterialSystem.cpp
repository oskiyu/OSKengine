#include "MaterialSystem.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

MaterialSystem::MaterialSystem(RenderAPI* renderer) {
	this->renderer = renderer;
}

MaterialSystem::~MaterialSystem() {
	for (auto& i : materials)
		delete i.second;

	materials.clear();
}

void MaterialSystem::RegisterMaterial(const std::string& name) {
	Material* material = new Material();
	material->SetRenderer(renderer);

	for (auto i : renderpasses)
		material->renderpassesToRegister.push_back(i);

	if (materials.find(name) != materials.end())
		delete materials[name];

	materials[name] = material;
}

Material* MaterialSystem::GetMaterial(const std::string& name) {
	if (materials.find(name) == materials.end())
		return nullptr;

	return materials[name];
}

void MaterialSystem::RegisterRenderpass(Renderpass* renderpass) {
	for (auto& i : materials)
		i.second->RegisterRenderpass(renderpass);

	renderpasses.push_back(renderpass);
}

void MaterialSystem::UnregisterRenderpass(Renderpass* renderpass) {
	for (auto& i : materials)
		i.second->UnregisterRenderpass(renderpass);

	renderpasses.remove(renderpass);
}
