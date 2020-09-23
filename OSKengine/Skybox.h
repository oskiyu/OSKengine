#pragma once

#include "Texture.h"
#include "Model.h"
#include "GraphicsPipeline.h"
#include "DescriptorSet.h"

namespace OSK {

	//Representa un skybox.
	class Skybox {

		friend class VulkanRenderer;
		friend class ContentManager;

	public:

		//Enlaza el skybox - tanto el modelo como la textura.
		inline void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, const uint32_t& i) const {
			Skybox::Model->Bind(commandBuffer);
			texture->Descriptor->Bind(commandBuffer, pipeline, i);
		}

		//Renderiza el skybox.
		inline void Draw(VkCommandBuffer commandBuffer) const {
			Skybox::Model->Draw(commandBuffer);
		}

	private:

		//Textura del skybox.
		SkyboxTexture* texture = nullptr;

		//Modelo del skybox (un cubo).
		//STATIC.
		static ModelData* Model;

	};

}