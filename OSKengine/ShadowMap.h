#pragma once

#include <vulkan/vulkan.h>
#include <cmath>

#include "GPUImage.h"
#include "Renderpass.h"
#include "GraphicsPipeline.h"
#include "RenderTarget.h"
#include "LightsUBO.h"

namespace OSK {

	/// <summary>
	/// Uniform buffer con la matriz de la cámara de la luz direccional.
	/// </summary>
	struct DirLightShadowUBO {

		/// <summary>
		/// Matriz de la cámara de la luz direccional.
		/// </summary>
		alignas(16) glm::mat4 DirLightMat = glm::mat4(1.0f);

	};

	/// <summary>
	/// Formato de la imagen del shadow map.
	/// </summary>
	constexpr auto SHADOW_MAP_FORMAT = VK_FORMAT_D32_SFLOAT;

	/// <summary>
	/// Filtro de la imagen del shadow map.
	/// </summary>
	constexpr auto SHADOW_MAP_FILTER = VK_FILTER_NEAREST;


	/// <summary>
	/// Clase que maneja el renderizado de sombras de una luz direccional.
	/// Contiene un render target en el que se renderiza información de las sombras.
	/// </summary>
	class OSKAPI_CALL ShadowMap {

		friend class RenderizableScene;

	public:

		/// <summary>
		/// Crea un shadow map vacío.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		/// <param name="content">Content manager para cargar y descargar el render target.</param>
		ShadowMap(RenderAPI* renderer, ContentManager* content);

		/// <summary>
		/// Limpia el shadow map.
		/// </summary>
		~ShadowMap();

		/// <summary>
		/// Crea el shadow map, con la resolución dada.
		/// </summary>
		/// <param name="size">Resolución del shadow map.</param>
		void Create(const Vector2ui& size);

		/// <summary>
		/// Limpia el shadow map.
		/// </summary>
		void Clear();

		/// <summary>
		/// UBOs con el DirShadowsUBO.
		/// </summary>
		std::vector<GPUDataBuffer>& GetUniformBuffers();

		/// <summary>
		/// Render target sobre el que se renderiza las sombras.
		/// </summary>
		RenderTarget* GetRenderTarget();

		/// <summary>
		/// Resolución del shadow map.
		/// </summary>
		Vector2ui GetImageSize() const;

		/// <summary>
		/// Inicio del rango de la cámara para las sombras.
		/// </summary>
		float depthRangeNear = 0.0f;

		/// <summary>
		/// Fin del rango de la cámara para las sombras.
		/// </summary>
		float depthRangeFar = 100.0f;

	//private:

		/// <summary>
		/// Render target sobre el que se renderiza las sombras.
		/// </summary>
		RenderTarget* dirShadows = nullptr;

		/// <summary>
		/// Resolución del shadow map.
		/// </summary>
		Vector2ui size;

		/// <summary>
		/// UBOs con el DirShadowsUBO.
		/// </summary>
		std::vector<GPUDataBuffer> dirShadowsUniformBuffers;

		/// <summary>
		/// Información que se manda a la GPU para el renderizado de las sombras.
		/// </summary>
		DirLightShadowUBO dirShadowsUBO = {};

		/// <summary>
		/// 
		/// </summary>
		float density = 20;

		/// <summary>
		/// Referencia a la luz direccional original.
		/// </summary>
		LightUBO* lights = nullptr;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

		/// <summary>
		/// Crea los uniform buffers.
		/// </summary>
		void CreateBuffers();

		/// <summary>
		/// Actualiza los uniform buffers.
		/// </summary>
		void UpdateBuffers();

		/// <summary>
		/// Actualiza la cámara del shadow map.
		/// </summary>
		void Update();

		/// <summary>
		/// Crea el renderpass para el renderizado de las sombras.
		/// </summary>
		void CreateRenderpass();

		/// <summary>
		/// Crea los framebuffers para el renderizado de las sombras.
		/// </summary>
		void CreateFramebuffers();

		/// <summary>
		/// Referencia al content manager.
		/// </summary>
		ContentManager* content = nullptr;

	};

}
