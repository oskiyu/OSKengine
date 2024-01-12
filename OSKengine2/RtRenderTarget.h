#pragma once

#include "RenderTargetAttachment.h"
#include "Vector2.hpp"

#include "RenderTargetAttachmentInfo.h"
#include "MaterialInstance.h"

namespace OSK::GRAPHICS {

	class IMaterialSlot;

	/// @brief Render target para el renderizado de im�genes desde shaders de trazado
	/// de rayos / computaci�n.
	class OSKAPI_CALL RtRenderTarget {

	public:

		/// @brief Crea el render target con la informaci�n dada.
		/// @param targetSize Resoluci�n, en p�xeles.
		/// @param attachmentInfo Informaci�n de la imagen que se va a generar.
		/// 
		/// @post El render target podr� usarse como storage image.
		/// 
		/// @note La imagen tendr� al menos GpuImageUsage::COMPUTE | GpuImageUsage::SAMPLED.
		void Create(const Vector2ui& targetSize, RenderTargetAttachmentInfo attachmentInfo);

		/// <summary> Cambia de resoluci�n la imagen del render target. </summary>
		/// <param name="targetSize">Nueva resoluci�n, en p�xeles.</param>
		/// 
		/// @warning Dejar� en un estado inv�lido cualquier material slot en el que se use
		/// esta imagen. 
		
		/// @brief Cambia de tama�o la imagen de renderizado.
		/// @param targetSize Nueva resoluci�n de la imagen de renderizado.
		/// 
		/// @pre El render target debe haber sido correctamente creado mediante
		/// RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// 
		/// @warning Esta funci�n destruye las im�genes anteriores, por lo que si una instancia de material
		/// referenciaba estas im�genes, se deber� actualizar con las nuevas im�genes.
		void Resize(const Vector2ui& targetSize);


		/// @brief Devuelve la imagen de renderizado en el �ndice de recursos dado.
		/// @return Puntero no nulo si fue correctamente inicializado. 
		/// Nullptr si el render target no fue inicializado.
		GpuImage* GetTargetImage();

		/// @brief Devuelve la imagen de renderizado en el �ndice de recursos dado.
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


		/// @brief Resoluci�n de la imagen, en p�xeles.
		/// @return Resoluci�n de la imagen, en p�xeles.
		Vector2ui GetSize() const;

	private:

		void SetupSpriteMaterial();

		RenderTargetAttachment m_attachment;

		UniquePtr<MaterialInstance> m_fullscreenSpriteMaterialInstance;

	};


	/// @brief Render target para el renderizado de im�genes desde shaders de trazado
	/// de rayos / computaci�n.
	using ComputeRenderTarget = RtRenderTarget;

}
