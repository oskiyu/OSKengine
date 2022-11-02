#pragma once

#include "RenderTargetAttachment.h"
#include "Vector2.hpp"

#include "RenderTargetAttachmentInfo.h"

namespace OSK::GRAPHICS {

	class MaterialInstance;
	class IMaterialSlot;

	/// <summary>
	/// Render target para el renderizado de imágenes desde shaders de trazado
	/// de rayos / computación.
	/// </summary>
	class OSKAPI_CALL RtRenderTarget {

	public:

		/// @brief Crea el render target con la información dada.
		/// @param targetSize Resolución, en píxeles.
		/// @param attachmentInfo Información de la imagen que se va a generar.
		/// 
		/// @note La imagen tendrá al menos GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED.
		void Create(const Vector2ui& targetSize, RenderTargetAttachmentInfo attachmentInfo);

		/// <summary> Cambia de resolución la imagen del render target. </summary>
		/// <param name="targetSize">Nueva resolución, en píxeles.</param>
		/// 
		/// @warning Dejará en un estado inválido cualquier material slot en el que se use
		/// esta imagen. 
		void Resize(const Vector2ui& targetSize);


		/// @brief Devuelve la imagen de renderizado en el índice de recursos dado.
		/// @param index Índice del frame.
		/// @return Nullptr si el render target no fue inicializado.
		/// @pre index < NUM_RESOURCES_IN_FLIGHT
		GpuImage* GetTargetImage(TSize index) const;


		/// <summary>
		/// Devuelve el material instance para el renderizado a pantalla completa usando
		/// el material IRenderer::GetFullscreenRenderingMaterial().
		/// </summary>
		/// <returns>Nullptr si no ha sido inicializado.</returns>
		MaterialInstance* GetFullscreenSpriteMaterialInstance() const;

		/// <summary>
		/// Devuelve el material slot requerido para el renderizado a pantalla completa usando
		/// el material IRenderer::GetFullscreenRenderingMaterial().
		/// </summary>
		/// <pre>Debe haberse inicializado.</pre>
		IMaterialSlot* GetFullscreenSpriteMaterialSlot() const;


		/// <summary> Resolución de las imágenes, en píxeles. </summary>
		Vector2ui GetSize() const;

	private:

		void SetupSpriteMaterial();

		RenderTargetAttachment attachment;

		UniquePtr<MaterialInstance> fullscreenSpriteMaterialInstance;

	};

	/// <summary>
	/// Render target para el renderizado de imágenes desde shaders de trazado
	/// de rayos / computación.
	/// </summary>
	using ComputeRenderTarget = RtRenderTarget;

}
