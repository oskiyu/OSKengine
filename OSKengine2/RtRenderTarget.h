#pragma once

#include "RenderTargetAttachment.h"
#include "Vector2.hpp"

#include "RenderTargetAttachmentInfo.h"

namespace OSK::GRAPHICS {

	class MaterialInstance;
	class IMaterialSlot;

	/// <summary>
	/// Render target para el renderizado de im�genes desde shaders de trazado
	/// de rayos / computaci�n.
	/// </summary>
	class OSKAPI_CALL RtRenderTarget {

	public:

		/// @brief Crea el render target con la informaci�n dada.
		/// @param targetSize Resoluci�n, en p�xeles.
		/// @param attachmentInfo Informaci�n de la imagen que se va a generar.
		/// 
		/// @note La imagen tendr� al menos GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED.
		void Create(const Vector2ui& targetSize, RenderTargetAttachmentInfo attachmentInfo);

		/// <summary> Cambia de resoluci�n la imagen del render target. </summary>
		/// <param name="targetSize">Nueva resoluci�n, en p�xeles.</param>
		/// 
		/// @warning Dejar� en un estado inv�lido cualquier material slot en el que se use
		/// esta imagen. 
		void Resize(const Vector2ui& targetSize);


		/// @brief Devuelve la imagen de renderizado en el �ndice de recursos dado.
		/// @param index �ndice del frame.
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


		/// <summary> Resoluci�n de las im�genes, en p�xeles. </summary>
		Vector2ui GetSize() const;

	private:

		void SetupSpriteMaterial();

		RenderTargetAttachment attachment;

		UniquePtr<MaterialInstance> fullscreenSpriteMaterialInstance;

	};

	/// <summary>
	/// Render target para el renderizado de im�genes desde shaders de trazado
	/// de rayos / computaci�n.
	/// </summary>
	using ComputeRenderTarget = RtRenderTarget;

}
