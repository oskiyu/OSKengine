#pragma once

#include "RenderTargetAttachment.h"
#include "Vector2.hpp"

#include "RenderTargetAttachmentInfo.h"
#include "MaterialInstance.h"

namespace OSK::GRAPHICS {

	class IMaterialSlot;

	/// @brief Render target para el renderizado de imágenes desde shaders de trazado
	/// de rayos / computación.
	class OSKAPI_CALL RtRenderTarget {

	public:

		/// @brief Crea el render target con la información dada.
		/// @param targetSize Resolución, en píxeles.
		/// @param attachmentInfo Información de la imagen que se va a generar.
		/// 
		/// @post El render target podrá usarse como storage image.
		/// 
		/// @note La imagen tendrá al menos GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED.
		void Create(const Vector2ui& targetSize, RenderTargetAttachmentInfo attachmentInfo);

		/// <summary> Cambia de resolución la imagen del render target. </summary>
		/// <param name="targetSize">Nueva resolución, en píxeles.</param>
		/// 
		/// @warning Dejará en un estado inválido cualquier material slot en el que se use
		/// esta imagen. 
		
		/// @brief Cambia de tamaño la imagen de renderizado.
		/// @param targetSize Nueva resolución de la imagen de renderizado.
		/// 
		/// @pre El render target debe haber sido correctamente creado mediante
		/// RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// 
		/// @warning Esta función destruye las imágenes anteriores, por lo que si una instancia de material
		/// referenciaba estas imágenes, se deberá actualizar con las nuevas imágenes.
		void Resize(const Vector2ui& targetSize);


		/// @brief Devuelve la imagen de renderizado en el índice de recursos dado.
		/// @return Puntero no nulo si fue correctamente inicializado. 
		/// Nullptr si el render target no fue inicializado.
		GpuImage* GetTargetImage();

		/// @brief Devuelve la imagen de renderizado en el índice de recursos dado.
		/// @return Puntero no nulo si fue correctamente inicializado. 
		/// Nullptr si el render target no fue inicializado.
		const GpuImage* GetTargetImage() const;


		/// @brief Devuelve el material instance para el renderizado a pantalla completa usando
		/// el material IRenderer::GetFullscreenRenderingMaterial().
		/// @return Nullptr si no ha sido inicializado, no nulo en caso contrario.
		MaterialInstance* GetFullscreenSpriteMaterialInstance() const;

		/// @brief Devuelve el material slot requerido para el renderizado a pantalla completa usando
		/// el material IRenderer::GetFullscreenRenderingMaterial().
		/// @return Puntero no nulo.
		/// 
		/// @pre Debe haberse inicializado correctamente.
		IMaterialSlot* GetFullscreenSpriteMaterialSlot() const;


		/// @brief Resolución de la imagen, en píxeles.
		/// @return Resolución de la imagen, en píxeles.
		Vector2ui GetSize() const;

	private:

		void SetupSpriteMaterial();

		RenderTargetAttachment m_attachment;

		UniquePtr<MaterialInstance> m_fullscreenSpriteMaterialInstance;

	};


	/// @brief Render target para el renderizado de imágenes desde shaders de trazado
	/// de rayos / computación.
	using ComputeRenderTarget = RtRenderTarget;

}
