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
#include "UniformBuffer.h"
#include "CubeShadowMap.h"

namespace OSK {

	class RenderAPI;

	/// <summary>
	/// Representa una escena que se puede renderizar.
	/// Contiene el terreno y los modelos renderizados.
	/// </summary>
	class OSKAPI_CALL RenderizableScene {

		friend class RenderAPI;
		friend class RenderSystem3D;
		friend class Scene;

	public:

		/// <summary>
		/// Crea una nueva escena.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		RenderizableScene(RenderAPI* renderer);

		/// <summary>
		/// Destruye la escena.
		/// </summary>
		~RenderizableScene();

		/// <summary>
		/// Establece las propiedades de las luces de la escena.
		/// El número de luces NO puede cambiar.
		/// Por defecto: 16 luces puntuales.
		/// </summary>
		void SetupLightsUBO();

		/// <summary>
		/// Actualiza los uniform buffers de la GPU que contienten información sobre las luces de la escena.
		/// </summary>
		void UpdateLightsBuffers();

		/// <summary>
		/// Añade un modelo a la escena.
		/// </summary>
		/// <param name="model">Modelo.</param>
		void AddModel(Model* model);

		/// <summary>
		/// Añade un modelo animado a la escena.
		/// </summary>
		/// <param name="model">Modelo animado.</param>
		void AddAnimatedModel(AnimatedModel* model);

		/// <summary>
		/// Carga el skybox que usará esta escena.
		/// Usa un ContentManager interno.
		/// </summary>
		/// <param name="path">Ruta del archivo (la carpeta).</param>
		void LoadSkybox(const std::string& path);

		/// <summary>
		/// Carga el terreno de l escena.
		/// Usa un ContentManager interno.
		/// </summary>
		/// <param name="path">Ruta del heightmap (con extensión).</param>
		/// <param name="quadSize">Tamaño de cada quad.</param>
		/// <param name="maxHeight">Altura máxima del terreno.</param>
		void LoadHeightmap(const std::string& path, const Vector2f& quadSize, float maxHeight);

		/// <summary>
		/// Luces de la escena.
		/// </summary>
		LightUBO lights = {};

		/// <summary>
		/// Terreno de la escena.
		/// </summary>
		UniquePtr<Terrain> terreno;

	//private:

		/// <summary>
		/// Renderiza todas las sombras de los modelos en el shadow map.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void DrawShadows(VkCommandBuffer cmdBuffer, uint32_t iteration);

		//void DrawPointShadows(VkCommandBuffer cmdBuffer, uint32_t iteration, CubeShadowMap* map);

		//Renderiza la escena.

		/// <summary>
		/// Renderiza toda la escena.
		/// Debe haberse enlazado un render target antes.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void Draw(VkCommandBuffer cmdBuffer, uint32_t iteration, RenderTarget* target);

		//SYSTEM

		/// <summary>
		/// Prepara el renderpass para renderizar las sombras.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void PrepareDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i);

		/// <summary>
		/// Renderiza en el shadow map la sombra de un modelo 3D.
		/// </summary>
		/// <param name="model">Modelo 3D.</param>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void DrawShadows(Model* model, VkCommandBuffer cmdBuffer, uint32_t i);

		/// <summary>
		/// Finaliza el renderizado de sombras.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void EndDrawShadows(VkCommandBuffer cmdBuffer, uint32_t i);

		/// <summary>
		/// Prepara la escena para su renderizado.
		/// Renderiza el skybox.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void PrepareDraw(VkCommandBuffer cmdBuffer, uint32_t i, RenderTarget* target);

		/// <summary>
		/// Renderiza un modelo.
		/// </summary>
		/// <param name="model">Modelo 3D.</param>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void Draw(Model* model, VkCommandBuffer cmdBuffer, uint32_t i);

		/// <summary>
		/// Finaliza el renderizado.
		/// Renderiza el terreno.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void EndDraw(VkCommandBuffer cmdBuffer, uint32_t i);

		/// <summary>
		/// Mapa de sombras.
		/// </summary>
		UniquePtr<ShadowMap> shadowMap;

		SharedPtr<UniformBuffer> uboLights = new UniformBuffer;
		SharedPtr<UniformBuffer> uboDirLightMat = new UniformBuffer;
		SharedPtr<UniformBuffer> uboBones = new UniformBuffer;

		/// <summary>
		/// Inicia los buffers de luces.
		/// </summary>
		void InitLightsBuffers();

		/// <summary>
		/// Graphics pipeline enlazada en un momento dado.
		/// </summary>
		GraphicsPipeline* currentGraphicsPipeline = nullptr;

		/// <summary>
		/// Pipeline del skybox.
		/// </summary>
		GraphicsPipeline* skyboxPipeline = nullptr;

		/// <summary>
		/// Content manager local.
		/// </summary>
		UniquePtr<ContentManager> content;

		/// <summary>
		/// Skybox.
		/// </summary>
		Skybox skybox;

		/// <summary>
		/// Modelos 3D.
		/// </summary>
		std::vector<Model*> models = {};

		/// <summary>
		/// Modelos animados.
		/// </summary>
		std::vector<AnimatedModel*> animatedModels = {};

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

		//std::vector<CubeShadowMap*> cubeShadowMaps;

		bool isPropetaryOfTerrain = true;
		
	};

}
