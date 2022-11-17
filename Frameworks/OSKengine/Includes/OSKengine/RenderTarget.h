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
	/// Render target para el renderizado de imágenes desde shaders de rasterización.
	/// </summary>
	class OSKAPI_CALL RenderTarget {

	public:

		/// <summary> Crea el render target con la información dada, para su uso como sampler. </summary>
		/// <param name="targetSize">Resolución, en píxeles.</param>
		/// <param name="colorInfos">Información de las imagenes de color.</param>
		/// <param name="depthInfo">Información de la imagen de profundidad.</param>
		void Create(const Vector2ui& targetSize, DynamicArray<RenderTargetAttachmentInfo> colorInfos, RenderTargetAttachmentInfo depthInfo);

		/// <summary> Crea el render target con la información dada, para su uso como imagen final de presentación. </summary>
		/// <param name="targetSize">Resolución, en píxeles.</param>
		/// <param name="colorInfo">Información de la imagen de color.</param>
		/// <param name="depthInfo">Información de la imagen de profundidad.</param>
		void CreateAsFinal(const Vector2ui& targetSize, RenderTargetAttachmentInfo colorInfo, RenderTargetAttachmentInfo depthInfo);


		/// <summary>
		/// Cambia de tamaño las imágenes de renderizado, tanto las de color
		/// como las de profundidad.
		/// </summary>
		/// <param name="targetSize">Nueva resolución de las imágenes de renderizado.</param>
		void Resize(const Vector2ui& targetSize);


		/// <summary> Devuelve la imagen de renderizado en los índices dados. </summary>
		/// <param name="colorImageIndex">Índice del target.</param>
		/// <param name="resourceIndex">Índice del frame.</param>
		/// @pre colorImageIndex debe apuntar a un target existente (colorImageIndex < RenderTarget::GetNumColorTargets).
		/// @pre colorImageIndex debe apuntar a un target existente (colorImageIndex < RenderTarget::GetNumColorTargets).
		GpuImage* GetColorImage(TIndex colorImageIndex, TIndex resourceIndex) const;

		/// <summary> Devuelve la imagen de renderizado principal (índice 0). </summary>
		/// <param name="resourceIndex">Índice del frame.</param>
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		GpuImage* GetMainColorImage(TIndex resourceIndex) const;

		/// <summary> Devuelve la imagen de profundidad. </summary>
		/// <param name="resourceIndex">Índice del frame.</param>
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		GpuImage* GetDepthImage(TIndex resourceIndex) const;


		/// <returns>
		/// Número de targets de color.
		/// Será 0 si no se ha inicializado.
		/// Será al menos 1 si se ha inicializado.
		/// </returns>
		TSize GetNumColorTargets() const;

		/// <summary> Resolución de las imágenes, en píxeles. </summary>
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
