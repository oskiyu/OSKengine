#pragma once

#include "IAssetLoader.h"

#include "UniquePtr.hpp"
#include "Material.h"
#include "MaterialInstance.h"
#include "RenderTarget.h"

#include <glm/glm.hpp>

namespace OSK::GRAPHICS {
	class ICommandList;
}

namespace OSK::ASSETS {

	class Model3D;

	/// <summary>
	/// Carga un mapa irradiance.
	/// </summary>
	/// 
	/// @see IrradianceMap.
	class OSKAPI_CALL IrradianceMapLoader : public IAssetLoader {

	public:

		IrradianceMapLoader();
		~IrradianceMapLoader();

		OSK_ASSET_TYPE_REG("OSK::IrradianceMap");

		void Load(const std::string& assetFilePath, IAsset** asset) override;

	private:

		/// <summary>
		/// Para poder generar el irradiance map, será necesario
		/// hacer varios renderizados a cubemaps.
		/// 
		/// Esta función auxiliar permite realizar un renderizado a
		/// un cubemap con los parámetros dados.
		/// </summary>
		/// 
		/// <param name="targetCubemap">Cubemap sobre el que se va a renderizar.</param>
		/// <param name="cmdList">Lista de comandos usada.</param>
		/// <param name="material">Pipeline usado para el renderizado.</param>
		/// <param name="materialSlot">Material slot usado durante el renderizado (con la imagen de entrada).</param>
		/// 
		/// @pre targetCubemap no debe ser nulo.
		/// @pre cmdList no debe ser nulo.
		/// @pre material no debe ser nulo.
		/// @pre materialSlot no debe ser nulo.
		void DrawCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList, GRAPHICS::Material* material, GRAPHICS::IMaterialSlot* materialSlot);


		/// <summary> Transforma una textura 2D en un cubemap </summary>
		/// <param name="targetCubemap">Cubemap que se generará.</param>
		/// 
		/// @pre targetCubemap no debe ser nulo.
		void GenCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList);

		/// <summary> Transforma un cubemap en un irradiance map. </summary>
		/// <param name="targetCubemap">Cubemap original.</param>
		/// 
		/// @pre targetCubemap no debe ser nulo.
		/// @pre cmdList no debe ser nulo.
		void ConvoluteCubemap(GRAPHICS::GpuImage* targetCubemap, GRAPHICS::ICommandList* cmdList);

		GRAPHICS::Material* cubemapGenMaterial = nullptr;
		GRAPHICS::Material* cubemapConvolutionMaterial = nullptr;

		UniquePtr<GRAPHICS::MaterialInstance> cubemapGenMaterialInstance = nullptr;
		UniquePtr<GRAPHICS::MaterialInstance> cubemapConvolutionMaterialInstance = nullptr;

		/// <summary>
		/// En este render target se renderiza el cubemap, que después
		/// se copiara a la GPU image de turno.
		/// </summary>
		GRAPHICS::RenderTarget cubemapGenRenderTarget;

		Model3D* cubemapModel = nullptr;

		const Vector2ui irradianceLayerSize = Vector2ui(512u, 512u);

		static glm::mat4 cameraProjection;
		static glm::mat4 cameraViews[6];

	};

}
