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

namespace OSK {

	class RenderAPI;

	//Representa una escena que se puede renderizar.
	class OSKAPI_CALL RenderizableScene {

		friend class RenderAPI;

	public:

		//Crea una nueva escena.
		RenderizableScene(RenderAPI* renderer);

		//Destruye la escena.
		~RenderizableScene();

		//Crea el descriptor layout de la escena.
		//Por defecto crea un PHONG descriptor layout.
		virtual void CreateDescriptorLayout();

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

		//Renderiza la escena.
		void Draw(VkCommandBuffer cmdBuffer, const uint32_t& iteration);

		//Carga un heightmap y genera un terreno.
		//	<path>: ruta del heightmap.
		//	<quadSize>: tamaño de cada cuadro del terreno (distancia entre los vértices).
		//	<maxHegith>: altura máxima del terreno.
		void LoadHeightmap(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight);
		
		//UBO de las luces.
		LightUBO Lights = {};

		//Terreno que se va a renderizar.
		Terrain* Terreno = nullptr;

	protected:

		void InitLightsBuffers();

		DescriptorLayout* PhongDescriptorLayout = nullptr;

		GraphicsPipeline* CurrentGraphicsPipeline = nullptr;
		GraphicsPipeline* SkyboxPipeline = nullptr;

		ContentManager* Content = nullptr;

		std::vector<VulkanBuffer> LightsUniformBuffers;

	private:

		ModelTexture* DefaultTexture = nullptr;

		Skybox skybox;

		std::vector<Model*> Models = {};
		std::vector<AnimatedModel*> AnimatedModels = {};

		RenderAPI* renderer;

		bool isPropetaryOfTerrain = true;

	};

}
