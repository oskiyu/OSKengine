#pragma once

#include "WindowAPI.h"
#include "Vector2.h"
#include <glm.hpp>

namespace OSK {

	struct OSKAPI_CALL UniformBuffer2D {
		glm::mat4 projection;
	};

	class OSKAPI_CALL Camera2D {
		friend class VulkanRenderer;

	public:

		Camera2D();

		Camera2D(WindowAPI* window);

		void Update();

		bool UseTargetSize = false;

		Vector2 TargetSize{};

	private:

		UniformBuffer2D getUBO();

		WindowAPI* window = nullptr;

		glm::mat4 projection;

	};

}