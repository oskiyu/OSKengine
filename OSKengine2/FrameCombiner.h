#pragma once

#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "RtRenderTarget.h"

namespace OSK::GRAPHICS {

	class IGpuImageView;
	class ICommandList;

	/// @brief Clase que permite combinar varias im�genes del mismo
	/// tama�o de una manera m�s eficiente.
	class OSKAPI_CALL FrameCombiner final {

	public:

		/// @brief Formatos soportados por el combiner.
		enum class ImageFormat {
			RGBA8,
			RGBA16
		};

	public:

		/// @brief Inicializa la clase.
		/// @param resolution Resoluci�n de la imagen, en p�xeles.
		/// @param info Informaci�n de la imagen.
		/// @note @p info.usage tendr� al menos GpuImageUsage::SAMPLED | GpuImageUsage::COMPUTE.
		void Create(const Vector2ui& resolution, RenderTargetAttachmentInfo info);

		/// @brief Cambia la resoluci�n de la imagen final.
		/// @param resolution Nueva resoluci�n, en p�xeles.
		void Resize(const Vector2ui& resolution);


		/// @brief Comienza el proceso de renderizado, sincronizando la imagen de destino.
		/// @param commandList Lista de comandos sobre la que se ejecutar�.
		/// @param format Formato de la imagen de salida.
		void Begin(ICommandList* commandList, ImageFormat format);

		/// @brief Renderiza una imagen 2D sobre este frame.
		/// @param image Imagen a a�adir.
		/// 
		/// @pre @p debe tener el layout GpuImageLayout::SAMPLED.
		/// @pre La resoluci�n de @p image debe ser igual a la resoluci�n de este frame.
		void Draw(ICommandList* commandList, const IGpuImageView& image);

		/// @brief Finaliza el renderizado.
		/// @param commandList Lista de comandos sobre la que se ejecutar�.
		/// @post El layout de GetCurrentImage() ser� GpuImageLayout::SAMPLED.
		void End(ICommandList* commandList);


		/// @return Barrier info que debe ser usado para la sincronizaci�n
		/// de las im�genes de entrada.
		static GpuBarrierInfo GetImageBarrierInfo();

		/// @return Imagen de renderizado del frame actual.
		GpuImage* GetCurrentImage() const;

		/// @param imageIndex �ndice del frame.
		/// @return Imagen de renderizado del frame.
		GpuImage* GetTargetImage(TIndex imageIndex) const;

		/// @return Render target sobre el que se construye la imagen final.
		const ComputeRenderTarget& GetRenderTarget() const;

	private:

		void LoadMaterial();
		void SetupTargetMaterial();

		void SetupTextureMaterialInstance(const IGpuImageView& image);

		Vector3ui GetDispatchResolution() const;

	private:

		ComputeRenderTarget renderTarget{};

		/// @brief Cada formato tiene su propio material.
		HashMap<ImageFormat, Material*> combinerMaterials;

		HashMap<const IGpuImageView*, UniquePtr<MaterialInstance>> textureMaterials[NUM_RESOURCES_IN_FLIGHT]{};
		UniquePtr<MaterialInstance> outputMaterialInstance = nullptr;

	};

}
