#include "ModelComponent.h"

using namespace OSK;

void ModelComponent::AddModel(const std::string& path, ContentManager* content) {
	staticMeshes.push_back({});
	content->LoadModel(&staticMeshes.back(), path);
}

void ModelComponent::AddAnimatedModel(const std::string& path, ContentManager* content) {
	animatedModels.push_back({});
	content->LoadAnimatedModel(&animatedModels.back(), path);
}

void ModelComponent::Link(Transform* transform) {
	for (auto& i : staticMeshes)
		i.GetTransform()->AttachTo(transform);

	for (auto& i : animatedModels)
		i.GetTransform()->AttachTo(transform);
}