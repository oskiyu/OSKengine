#include "MaterialSystem.h"

#include "VulkanRenderer.h"

using namespace OSK;
using namespace OSK::VULKAN;

MaterialSystem::MaterialSystem(RenderAPI* renderer) {
	Renderer = renderer;
}

MaterialSystem::~MaterialSystem() {
	for (auto& i : Materials) {
		delete i.second;
	}

	Materials.clear();
}

void MaterialSystem::RegisterMaterial(const std::string& name) {
	Material* material = new Material();
	material->SetRenderer(Renderer);

	for (auto i : Renderpasses)
		material->RenderpassesToRegister.push_back(i);

	if (Materials.find(name) != Materials.end())
		delete Materials[name];

	Materials[name] = material;
}

Material* MaterialSystem::GetMaterial(const std::string& name) {
	if (Materials.find(name) == Materials.end())
		return nullptr;

	return Materials[name];
}

void MaterialSystem::RegisterRenderpass(Renderpass* renderpass) {
	for (auto& i : Materials)
		i.second->RegisterRenderpass(renderpass);

	Renderpasses.push_back(renderpass);
}

void MaterialSystem::UnregisterRenderpass(Renderpass* renderpass) {
	for (auto& i : Materials)
		i.second->UnregisterRenderpass(renderpass);

	Renderpasses.remove(renderpass);
}
