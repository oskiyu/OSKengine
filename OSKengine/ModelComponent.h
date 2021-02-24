#pragma once

#include "ECS.h"
#include "AnimatedModel.h"
#include "ContentManager.h"

namespace OSK {

	class ModelComponent : public Component {

	public:

		std::vector<Model> StaticMeshes{};
		std::vector<AnimatedModel*> AnimatedModels{};

		void AddModel(const std::string& path, ContentManager* content) {
			StaticMeshes.push_back({});
			content->LoadModel(StaticMeshes.back(), path);
		}
		void AddAnimatedModel(const std::string& path, ContentManager* content) {
			AnimatedModels.push_back(content->LoadAnimatedModel(path));
		}

		void Link(Transform* transform) {
			for (auto& i : StaticMeshes)
				i.ModelTransform.AttachTo(transform);

			for (auto& i : AnimatedModels)
				i->ModelTransform.AttachTo(transform);
		}

		void Pack() {
			StaticMeshes.shrink_to_fit();
			AnimatedModels.shrink_to_fit();
		}

	};

}
