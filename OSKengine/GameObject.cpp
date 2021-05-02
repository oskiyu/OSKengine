#include "GameObject.h"

#include "ECS.h"
#include "ModelComponent.h"

using namespace OSK;

void GameObject::Remove() {
	Delete(this);
}

void GameObject::AddModel(const std::string& path, ContentManager* content) {
	if (!HasComponent<ModelComponent>())
		AddComponent<ModelComponent>({});

	GetComponent<ModelComponent>().AddModel(path, content);
}

void GameObject::AddAnimatedModel(const std::string& path, ContentManager* content) {
	if (!HasComponent<ModelComponent>())
		AddComponent<ModelComponent>({});

	GetComponent<ModelComponent>().AddAnimatedModel(path, content);
}