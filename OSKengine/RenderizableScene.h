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

#include "ShadowMap.h"
#include "CubeShadowMap.h"

namespace OSK {

	class RenderAPI;

	//Representa una escena que se puede renderizar.
	class OSKAPI_CALL RenderizableScene {

		friend class RenderAPI;
		friend class RenderSystem3D;

	public:

		//Crea una nueva escena.
		RenderizableScene(RenderAPI* renderer);

		//Destruye la escena.
		~RenderizableScene();

		//Establece las propiedades de las luces de la escena.
		//El número de luces NO puede cambiar.
		//Por defecto: 16 luces puntuales.
		virtual void SetupLightsUBO();

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

		void DrawShadows(VkCommandBuffer cmdBuffer, uint32_t iteration);
		void DrawPointShadows(VkCommandBuffer cmdBuffer, uint32_t iteration, CubeShadowMap* map);

		//Renderiza la escena.
		void Draw(VkCommandBuffer cmdBuffer, uint32_t iteration);

		//Carga un heightmap y genera un terreno.
		//	<path>: ruta del heightmap.
		//	<quadSize>: tamaño de cada cuadro del terreno (distancia entre los vértices).
		//	<maxHegith>: altura máxima del terreno.
		void LoadHeightmap(const std::string& path, const Vector2f& quadSize, float maxHeight);
		
		//UBO de las luces.
		LightUBO Lights = {};
		//Terreno que se va a renderizar.
		UniquePtr<Terrain> Terreno;

		//SYSTEM
		void PrepareDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i);
		void DrawShadows(Model* model, VkCommandBuffer cmdBuffer, uint32_t i);
		void EndDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i);

		void PrepareDraw(VkCommandBuffer cmdBuffer, uint32_t i);
		void Draw(Model* model, VkCommandBuffer cmdBuffer, uint32_t i);
		void EndDraw(VkCommandBuffer cmdBuffer, uint32_t i);

		VULKAN::Renderpass* TargetRenderpass = nullptr;
		
		UniquePtr<ShadowMap> shadowMap;
		std::vector<VulkanBuffer> LightsUniformBuffers;

		std::vector<VulkanBuffer> BonesUBOs;


	protected:

		void InitLightsBuffers();

		GraphicsPipeline* CurrentGraphicsPipeline = nullptr;
		GraphicsPipeline* SkyboxPipeline = nullptr;

		UniquePtr<ContentManager> Content;

	private:
		Skybox skybox;

		std::vector<Model*> Models = {};
		std::vector<AnimatedModel*> AnimatedModels = {};

		RenderAPI* renderer;
		std::vector<CubeShadowMap*> cubeShadowMaps;

		bool isPropetaryOfTerrain = true;
		
	};

}
