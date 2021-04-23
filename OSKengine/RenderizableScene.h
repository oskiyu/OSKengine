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

	/// <summary>
	/// Representa una escena que se puede renderizar.
	/// Contiene el terreno y los modelos renderizados.
	/// </summary>
	class OSKAPI_CALL RenderizableScene {

		friend class RenderAPI;
		friend class RenderSystem3D;

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
		/// El n�mero de luces NO puede cambiar.
		/// Por defecto: 16 luces puntuales.
		/// </summary>
		virtual void SetupLightsUBO();

		/// <summary>
		/// Actualiza los uniform buffers de la GPU que contienten informaci�n sobre las luces de la escena.
		/// </summary>
		void UpdateLightsBuffers();

		/// <summary>
		/// A�ade un modelo a la escena.
		/// </summary>
		/// <param name="model">Modelo.</param>
		void AddModel(Model* model);

		/// <summary>
		/// A�ade un modelo animado a la escena.
		/// </summary>
		/// <param name="model">Modelo animado.</param>
		void AddAnimatedModel(AnimatedModel* model);

		/// <summary>
		/// Carga el skybox que usar� esta escena.
		/// Usa un ContentManager interno.
		/// </summary>
		/// <param name="path">Ruta del archivo (la carpeta).</param>
		void LoadSkybox(const std::string& path);

		/// <summary>
		/// Renderiza todas las sombras de los modelos en el shadow map.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void DrawShadows(VkCommandBuffer cmdBuffer, uint32_t iteration);

		void DrawPointShadows(VkCommandBuffer cmdBuffer, uint32_t iteration, CubeShadowMap* map);

		//Renderiza la escena.

		/// <summary>
		/// Renderiza toda la escena.
		/// Debe haberse enlazado un render target antes.
		/// </summary>
		/// <param name="cmdBuffer">Commandbuffer.</param>
		/// <param name="iteration">Iteration.</param>
		void Draw(VkCommandBuffer cmdBuffer, uint32_t iteration);

		//Carga un heightmap y genera un terreno.
		//	<path>: ruta del heightmap.
		//	<quadSize>: tama�o de cada cuadro del terreno (distancia entre los v�rtices).
		//	<maxHegith>: altura m�xima del terreno.

		/// <summary>
		/// Carga el terreno de l escena.
		/// Usa un ContentManager interno.
		/// </summary>
		/// <param name="path">Ruta del heightmap (con extensi�n).</param>
		/// <param name="quadSize">Tama�o de cada quad.</param>
		/// <param name="maxHeight">Altura m�xima del terreno.</param>
		void LoadHeightmap(const std::string& path, const Vector2f& quadSize, float maxHeight);
		
		/// <summary>
		/// Luces de la escena.
		/// </summary>
		LightUBO Lights = {};

		/// <summary>
		/// Terreno de la escena.
		/// </summary>
		UniquePtr<Terrain> Terreno;

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
		void PrepareDraw(VkCommandBuffer cmdBuffer, uint32_t i);

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
		/// Renderpass.
		/// </summary>
		VULKAN::Renderpass* TargetRenderpass = nullptr;
		
		/// <summary>
		/// Mapa de sombras.
		/// </summary>
		UniquePtr<ShadowMap> shadowMap;

		/// <summary>
		/// Buffers con informaci�n de luces.
		/// </summary>
		std::vector<GPUDataBuffer> LightsUniformBuffers;

		/// <summary>
		/// Buffers con informaci�n de huesos de modelos animados.
		/// </summary>
		std::vector<GPUDataBuffer> BonesUBOs;


	protected:

		/// <summary>
		/// Inicia los buffers de luces.
		/// </summary>
		void InitLightsBuffers();

		/// <summary>
		/// Graphics pipeline enlazada en un momento dado.
		/// </summary>
		GraphicsPipeline* CurrentGraphicsPipeline = nullptr;

		/// <summary>
		/// Pipeline del skybox.
		/// </summary>
		GraphicsPipeline* SkyboxPipeline = nullptr;

		/// <summary>
		/// Content manager local.
		/// </summary>
		UniquePtr<ContentManager> Content;

	private:

		/// <summary>
		/// Skybox.
		/// </summary>
		Skybox skybox;

		/// <summary>
		/// Modelos 3D.
		/// </summary>
		std::vector<Model*> Models = {};

		/// <summary>
		/// Modelos animados.
		/// </summary>
		std::vector<AnimatedModel*> AnimatedModels = {};

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

		std::vector<CubeShadowMap*> cubeShadowMaps;

		bool isPropetaryOfTerrain = true;
		
	};

}
