#pragma once

#include "ApiCall.h"
#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

#include "IGpuImage.h"
#include "ImageUuid.h"
#include "RenderGraphArgs.h"
#include "RgEntry.h"

#include <span>
#include <unordered_map>

namespace OSK::GRAPHICS {

	class IRenderer;


	/// @brief Implementa el manejo de imágenes
	/// en el render-graph.
	/// 
	/// @note Es dueño de las imágenes.
	class OSKAPI_CALL RgImageManager {

	public:

		explicit RgImageManager(IRenderer* renderer);

		/// @brief Registra una nueva imagen, que
		/// será creada y gestionada por el render-graph.
		/// @param info Información de la imagen.
		/// @param size Resolución de la imagen, relativa a
		/// la resolución del framebuffer.
		/// 
		/// @return UUID de la imagen.
		GdrImageUuid RegisterImage(
			const RgImageRegisterInfo& info,
			RgRelativeSizeImageRegisterArgs size);

		/// @brief Registra una nueva imagen, que
		/// será creada y gestionada por el render-graph.
		/// @param info Información de la imagen.
		/// @param size Resolución de la imagen.
		/// 
		/// @return UUID de la imagen.
		GdrImageUuid RegisterImage(
			const RgImageRegisterInfo& info,
			RgAbsoluteSizeImageRegisterArgs size);

		/// @brief Registra una imagen que ha sido cargada
		/// externamente, y que por lo tanto no debe ser
		/// creada.
		/// @return UUID asignado a la imagen.
		GdrImageUuid RegisterExternalImage(GpuImage* image);


		/// @brief Crea las imágenes, teniendo en cuenta las
		/// dependencias declaradas en los renderpases.
		/// @param renderpasses Renderpases del render-graph.
		void CreateImages(std::span<const RenderPassEntry> renderpasses);

		GpuImage& GetImage(GdrImageUuid uuid);
		const GpuImage& GetImage(GdrImageUuid uuid) const;

		/// @brief Actualiza la referencia de tamaño
		/// del framebuffer, para poder cambiar la 
		/// resolución de las imágenes ligadas al
		/// framebuffer.
		/// @param res Nueva resolución.
		void SetFramebufferResolution(USize32   res);
		void SetFramebufferResolution(Vector2ui res);
		void SetFramebufferResolution(Vector3ui res);

	private:

		GdrImageUuid RegisterImage(
			const RgImageRegisterInfo& info,
			RgImageSizeInfo size);

		IRenderer* m_renderer = nullptr;

		Vector3ui m_framebufferResolution = Vector3ui::Zero;

		std::unordered_map<GdrImageUuid, GpuImage*>  m_imagesMap;
		DynamicArray<UniquePtr<GpuImage>> m_images;

		std::unordered_map<GdrImageUuid, RgImageSizeInfo> m_imageSizes;
		std::unordered_map<GdrImageUuid, RgImageRegisterInfo> m_imageInfos;

	};

}
