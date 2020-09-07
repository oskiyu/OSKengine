#include "Texture.h"

namespace OSK {

	ModelTexture::ModelTexture(VkDevice logicalDevice) {
		this->logicalDevice = logicalDevice;

		Albedo.logicalDevice = &this->logicalDevice;
		Specular.logicalDevice = &this->logicalDevice;
	}

	ModelTexture::~ModelTexture() {
		Albedo.Destroy();
		Specular.Destroy();
	}

}