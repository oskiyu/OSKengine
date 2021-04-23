#include "ModelComponent.h"

using namespace OSK;

void ModelComponent::AddModel(const std::string& path, ContentManager* content) {
	StaticMeshes.push_back({});
	content->LoadModel(StaticMeshes.back(), path);
}

void ModelComponent::AddAnimatedModel(const std::string& path, ContentManager* content) {
	AnimatedModels.push_back({});
	content->LoadAnimatedModel(AnimatedModels.back(), path);
}

void ModelComponent::Link(Transform* transform) {
	for (auto& i : StaticMeshes)
		i.ModelTransform.AttachTo(transform);

	for (auto& i : AnimatedModels)
		i.ModelTransform.AttachTo(transform);
}
