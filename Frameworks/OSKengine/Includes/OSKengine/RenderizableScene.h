#pragma once

#include <vector>

#include "Model.h"
#include "AnimatedModel.h"
#include "Skybox.h"
#include "ContentManager.h"
#include "GraphicsPipeline.h"
#include "LightsUBO.h"
#include "Terrain.h"
#include "DescriptorLayout.h"
#include "DescriptorSet.h"
#include "RenderTarget.h"
#include "GraphicsPipeline.h"

namespace OSK {

	class RenderAPI;


	struct DirLightShadowUBO {
		glm::mat4 DirLightMat = glm::mat4(1.0f);
	};


	//Representa una escena que se puede renderizar.
	class OSKAPI_CALL RenderizableScene {

		friend class RenderAPI;

	public:

		//Crea una nueva escena.
		RenderizableScene(RenderAPI* renderer, uint32_t maxInitEntities);

		//Destruye la escena.
		~RenderizableScene();

		//Crea el descriptor layout de la escena.
		//Por defecto crea un PHONG descriptor layout.
		virtual void CreateDescriptorLayout(uint32_t maxSets);

		//Establece las propiedades de las luces de la escena.
		//El número de luces NO puede cambiar.
		//Por defecto: 16 luces puntuales.
		virtual void SetupLightsUBO();

		//Crea un descriptor set para la textura dada.
		//	<texture>: textura del modelo.
		//Por defecto crea un PHONG descriptor set.
		virtual void CreateDescriptorSet(ModelTexture* texture) const;

		//Crea el GraphicsPipeline de la escena.
		//Por defecto crea un PHONG GraphicsPipeline.
		virtual void SetGraphicsPipeline();

		//Recrea el GraphicsPipeline.
		void RecreateGraphicsPipeline();

		//Actualiza los uniform buffers de la GPU que contienten información sobre las luces de la escena.
		void UpdateLightsBuffers();

		//Añade un modelo a la escena.
		//	<model>: modelo.
		void AddModel(Model* model);

		//Añade un modelo animado a la escena.
		//	<model>: modelo animado.
		void AddAnimatedModel(AnimatedModel* model);

		//Carga el skybox que usará esta escena.
		//	<path>: ruta del archivo.
		void LoadSkybox(const std::string& path);

		void DrawShadows(VkCommandBuffer cmdBuffer, const uint32_t& iteration);

		//Renderiza la escena.
		void Draw(VkCommandBuffer cmdBuffer, const uint32_t& iteration);

		//Carga un heightmap y genera un terreno.
		//	<path>: ruta del heightmap.
		//	<quadSize>: tamaño de cada cuadro del terreno (distancia entre los vértices).
		//	<maxHegith>: altura máxima del terreno.
		void LoadHeightmap(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight);
		
		//UBO de las luces.
		LightUBO Lights = {};
		DirLightShadowUBO DirShadowsUBO = {};

		//Terreno que se va a renderizar.
		Terrain* Terreno = nullptr;

	protected:

		void InitLightsBuffers();

		DescriptorLayout* PhongDescriptorLayout = nullptr;
		DescriptorLayout* DirShadowDescriptorLayout = nullptr;

		GraphicsPipeline* CurrentGraphicsPipeline = nullptr;
		GraphicsPipeline* SkyboxPipeline = nullptr;

		ContentManager* Content = nullptr;

		std::vector<VulkanBuffer> LightsUniformBuffers;
		std::vector<VulkanBuffer> DirShadowsUniformBuffers;

		RenderTarget* DirShadows = nullptr;
		GraphicsPipeline* ShadowsPipeline = nullptr;

	private:

		ModelTexture* DefaultTexture = nullptr;

		Skybox skybox;

		std::vector<Model*> Models = {};
		std::vector<AnimatedModel*> AnimatedModels = {};

		RenderAPI* renderer;

		bool isPropetaryOfTerrain = true;

	};

}
