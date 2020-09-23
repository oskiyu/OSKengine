#pragma once

#include <vector>

#include "Model.h"
#include "AnimatedModel.h"
#include "Skybox.h"
#include "ContentManager.h"
#include "GraphicsPipeline.h"
#include "LightsUBO.h"
#include "Terrain.h"

namespace OSK {

	class VulkanRenderer;

	//Representa una escena que se puede renderizar.
	class RenderizableScene {

		friend class VulkanRenderer;

	public:

		//Crea una nueva escena.
		RenderizableScene(VulkanRenderer* renderer);

		//Destruye la escena.
		~RenderizableScene();

		//Añade un modelo a la escena.
		//	<model>: modelo.
		void AddModel(Model* model);

		//Añade un modelo animado a la escena.
		//	<model>: modelo animado.
		void AddAnimatedModel(AnimatedModel* model);

		//Carga el skybox que usará esta escena.
		//	<path>: ruta del archivo.
		void LoadSkybox(const std::string& path);

		//Establece la GraphicsPipeline que usará esta escena.
		//	<pipeline>: pipeline.
		void SetGraphicsPipeline(GraphicsPipeline* pipeline);

		//Crea un descriptor set PHONG que usará el LughtsUBO de esta escena.
		//	<texture>: textura del modelo.
		//	<albedoSampler>: sampler de la textura (albedo).
		//	<specularSampler>: sampler de la textura (specular).
		void CreatePhongDescriptorSet(ModelTexture* texture, VkSampler albedoSampler, VkSampler specularSampler) const;
		//virtual void CreateDescriptorSet(ModelTexture* texture) const = 0;

		//Renderiza la escena.
		void Draw(VkCommandBuffer cmdBuffer, const uint32_t& iteration);

		//Carga un heightmap y genera un terreno.
		//	<path>: ruta del heightmap.
		//	<quadSize>: tamaño de cada cuadro del terreno (distancia entre los vértices).
		//	<maxHegith>: altura máxima del terreno.
		void LoadHeightmap(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight);
		
		LightUBO Lights = {};

		Terrain* terreno;

	protected:

		GraphicsPipeline* CurrentGraphicsPipeline = nullptr;

		GraphicsPipeline* PhongPipeline = nullptr;
		GraphicsPipeline* PBR_Pipeline = nullptr;
		GraphicsPipeline* SkyboxPipeline = nullptr;

		ContentManager* Content = nullptr;

		std::vector<VulkanBuffer> LightsUniformBuffers;

	private:

		Skybox skybox;

		std::vector<Model*> Models = {};
		std::vector<AnimatedModel*> AnimatedModels = {};

		VulkanRenderer* renderer;

		bool isPropetaryOfTerrain = true;

	};

}
