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

	
	/// @brief Render target para el renderizado de imágenes desde shaders de rasterizado.
	class OSKAPI_CALL RenderTarget {

	public:

		/// @brief Crea el render target con la información dada, para su uso como render target
		/// en shaders de fragmentos.
		/// @param targetSize Resolución, en píxeles.
		/// @param colorInfos Información de las imágenes finales de color.
		/// @param depthInfo Información de la imagen de profundidad.
		/// 
		/// @pre Debe haber información para al menos una imagen de color.
		/// 
		/// @post El render target podrá usarse como renderpass para shaders de fragmentos.
		/// 
		/// @note Pueden incluirse varias imágenes de color objetivo.
		/// @note Cada imagen objetivo de color es independiente y puede tener formatos y usos distintos al resto
		/// de imágenes objetivo.
		/// 
		/// @throws InvalidArgumentException Si no se cumple la primera precondición.
		void Create(const Vector2ui& targetSize, DynamicArray<RenderTargetAttachmentInfo> colorInfos, RenderTargetAttachmentInfo depthInfo);

		/// @brief Crea el render target con la información dada, para su uso como imagen final de presentación.
		/// @param targetSize Resolución, en píxeles
		/// @param colorInfo Información de la imagen de color.
		/// @param depthInfo Información de la imagen de profundidad.
		/// 
		/// @post El render target podrá usarse como renderpass para shaders de fragmentos.
		/// 
		/// @note Para uso interno, no usar en producción.
		void CreateAsFinal(const Vector2ui& targetSize, RenderTargetAttachmentInfo colorInfo, RenderTargetAttachmentInfo depthInfo);


		/// @brief Cambia de tamaño las imágenes de renderizado, tanto las de color
		/// como las de profundidad.
		/// @param targetSize Nueva resolución de las imágenes de renderizado.
		/// 
		/// @pre El render target debe haber sido correctamente creado mediante
		/// RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// 
		/// @warning Esta función destruye las imágenes anteriores, por lo que si una instancia de material
		/// referenciaba estas imágenes, se deberá actualizar con las nuevas imágenes.
		void Resize(const Vector2ui& targetSize);


		/// @brief Devuelve la imagen de renderizado en los índices dados.
		/// @param colorImageIndex Índice de la imagen objetivo de color.
		/// @param resourceIndex Índice del frame.
		/// @return Puntero no nulo a la imagen.
		/// 
		/// @pre Este render target debe de haberse inicializado previamente con 
		/// RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// @pre colorImageIndex debe apuntar a un target existente (colorImageIndex < RenderTarget::GetNumColorTargets).
		/// @pre resourceIndex debe estar entre 0 y NUM_RESOURCES_IN_FLIGHT 
		/// (0 <= resourceIndex < NUM_RESOURCES_IN_FLIGHT).
		/// 
		/// @throws InvalidArgumentException si se incumple alguna precondición.
		GpuImage* GetColorImage(UIndex32 colorImageIndex, UIndex32 resourceIndex) const;

		/// @brief Devuelve la imagen de renderizado principal (índice 0).
		/// @param resourceIndex Índice del frame.
		/// @return Puntero no nulo a la imagen.
		/// 
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// 
		/// @throws std::runtime_error si se incumple la precondición.
		GpuImage* GetMainColorImage(UIndex32 resourceIndex) const;

		/// @brief Devuelve la imagen de profundidad.
		/// @param resourceIndex Índice del frame.
		/// @return Puntero no nulo a la imagen.
		/// 
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// @pre resourceIndex debe estar entre 0 y NUM_RESOURCES_IN_FLIGHT 
		/// (0 <= resourceIndex < NUM_RESOURCES_IN_FLIGHT).
		/// 
		/// @throws InvalidArgumentException Si se incumple alguna de las precondiciones.
		GpuImage* GetDepthImage(UIndex32 resourceIndex) const;


		/// @brief Número de targets de color.
		/// @return Número de imágenes objetivo de color.
		/// 
		/// @remark Será 0 si no se ha inicializado.
		/// @remark Será al menos 1 si se ha inicializado.
		USize32 GetNumColorTargets() const;

		/// @brief Devuelve la resolución de todas las imágenes, en píxeles.
		/// @return Resolución del render target.
		/// 
		/// @pre Debe haberse inicializado correctamente.
		Vector2ui GetSize() const;


		/// @brief Devuelve el material instance para el renderizado a pantalla completa usando
		/// el material IRenderer::GetFullscreenRenderingMaterial().
		/// @return Nullptr si no ha sido inicializado, no nulo en caso contrario.
		MaterialInstance* GetFullscreenSpriteMaterialInstance();

		/// @brief Devuelve el material slot requerido para el renderizado a pantalla completa usando
		/// el material IRenderer::GetFullscreenRenderingMaterial().
		/// @return Puntero no nulo.
		/// 
		/// @pre Debe haberse inicializado correctamente.
		IMaterialSlot* GetFullscreenSpriteMaterialSlot();


		/// @brief Devuelve el tipo de render target (intermedio para ser usado
		/// con el sprite, o final para ser renderizado en la propia pantalla).
		/// @return Tipo de render target.
		RenderpassType GetRenderTargetType() const;

	private:

		void SetupSpriteMaterial();

		DynamicArray<RenderTargetAttachment> colorAttachments;
		RenderTargetAttachment depthAttachment;

		RenderpassType targetType = RenderpassType::INTERMEDIATE;

		UniquePtr<MaterialInstance> fullscreenSpriteMaterialInstance;

	};

}
