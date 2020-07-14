#pragma once

#include <vector>
#include "VulkanObject.hpp"

class Scene {

public:

	std::vector<VulkanObject*> Objects;

	VulkanObject* CreateObject() {
		VulkanObject obj{};
		Objects.push_back(&obj);

		return &obj;
	}

	void Reorder() {

	}

};
