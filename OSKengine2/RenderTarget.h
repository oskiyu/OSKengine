#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "Vector2.hpp"

#include "Sprite.h"
#include "Transform2D.h"
#include "RenderpassType.h"

#include "RenderTargetAttachment.h"
#include "RenderTargetAttachmentInfo.h"

#include "MaterialInstance.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Render target para el renderizado de im�genes desde shaders de rasterizaci�n.
	/// </summary>
	class OSKAPI_CALL RenderTarget {

	public:

		/// <summary> Crea el render target con la informaci�n dada, para su uso como sampler. </summary>
		/// <param name="targetSize">Resoluci�n, en p�xeles.</param>
		/// <param name="colorInfos">Informaci�n de las imagenes de color.</param>
		/// <param name="depthInfo">Informaci�n de la imagen de profundidad.</param>
		void Create(const Vector2ui& targetSize, DynamicArray<RenderTargetAttachmentInfo> colorInfos, RenderTargetAttachmentInfo depthInfo);

		/// <summary> Crea el render target con la informaci�n dada, para su uso como imagen final de presentaci�n. </summary>
		/// <param name="targetSize">Resoluci�n, en p�xeles.</param>
		/// <param name="colorInfo">Informaci�n de la imagen de color.</param>
		/// <param name="depthInfo">Informaci�n de la imagen de profundidad.</param>
		void CreateAsFinal(const Vector2ui& targetSize, RenderTargetAttachmentInfo colorInfo, RenderTargetAttachmentInfo depthInfo);


		/// <summary>
		/// Cambia de tama�o las im�genes de renderizado, tanto las de color
		/// como las de profundidad.
		/// </summary>
		/// <param name="targetSize">Nueva resoluci�n de las im�genes de renderizado.</param>
		void Resize(const Vector2ui& targetSize);


		/// <summary> Devuelve la imagen de renderizado en los �ndices dados. </summary>
		/// <param name="colorImageIndex">�ndice del target.</param>
		/// <param name="resourceIndex">�ndice del frame.</param>
		/// @pre colorImageIndex debe apuntar a un target existente (colorImageIndex < RenderTarget::GetNumColorTargets).
		/// @pre colorImageIndex debe apuntar a un target existente (colorImageIndex < RenderTarget::GetNumColorTargets).
		GpuImage* GetColorImage(TIndex colorImageIndex, TIndex resourceIndex) const;

		/// <summary> Devuelve la imagen de renderizado principal (�ndice 0). </summary>
		/// <param name="resourceIndex">�ndice del frame.</param>
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		GpuImage* GetMainColorImage(TIndex resourceIndex) const;

		/// <summary> Devuelve la imagen de profundidad. </summary>
		/// <param name="resourceIndex">�ndice del frame.</param>
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		GpuImage* GetDepthImage(TIndex resourceIndex) const;


		/// <returns>
		/// N�mero de targets de color.
		/// Ser� 0 si no se ha inicializado.
		/// Ser� al menos 1 si se ha inicializado.
		/// </returns>
		TSize GetNumColorTargets() const;

		/// <summary> Resoluci�n de las im�genes, en p�xeles. </summary>
		Vector2ui GetSize() const;


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


		/// <summary>
		/// Devuelve el tipo de render target (intermedio para ser usado
		/// con el sprite, o final para ser renderizado en la propia pantalla).
		/// </summary>
		RenderpassType GetRenderTargetType() const;

	private:

		void SetupSpriteMaterial();

		DynamicArray<RenderTargetAttachment> colorAttachments;
		RenderTargetAttachment depthAttachment;

		RenderpassType targetType = RenderpassType::INTERMEDIATE;

		UniquePtr<MaterialInstance> fullscreenSpriteMaterialInstance;

	};

}
