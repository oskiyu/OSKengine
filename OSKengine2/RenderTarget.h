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

	
	/// @brief Render target para el renderizado de im�genes desde shaders de rasterizado.
	class OSKAPI_CALL RenderTarget {

	public:

		/// @brief Crea el render target con la informaci�n dada, para su uso como render target
		/// en shaders de fragmentos.
		/// @param targetSize Resoluci�n, en p�xeles.
		/// @param colorInfos Informaci�n de las im�genes finales de color.
		/// @param depthInfo Informaci�n de la imagen de profundidad.
		/// 
		/// @pre Debe haber informaci�n para al menos una imagen de color.
		/// 
		/// @post El render target podr� usarse como renderpass para shaders de fragmentos.
		/// 
		/// @note Pueden incluirse varias im�genes de color objetivo.
		/// @note Cada imagen objetivo de color es independiente y puede tener formatos y usos distintos al resto
		/// de im�genes objetivo.
		/// 
		/// @throws InvalidArgumentException Si no se cumple la primera precondici�n.
		void Create(const Vector2ui& targetSize, DynamicArray<RenderTargetAttachmentInfo> colorInfos, RenderTargetAttachmentInfo depthInfo);

		/// @brief Crea el render target con la informaci�n dada, para su uso como imagen final de presentaci�n.
		/// @param targetSize Resoluci�n, en p�xeles
		/// @param colorInfo Informaci�n de la imagen de color.
		/// @param depthInfo Informaci�n de la imagen de profundidad.
		/// 
		/// @post El render target podr� usarse como renderpass para shaders de fragmentos.
		/// 
		/// @note Para uso interno, no usar en producci�n.
		void CreateAsFinal(const Vector2ui& targetSize, RenderTargetAttachmentInfo colorInfo, RenderTargetAttachmentInfo depthInfo);


		/// @brief Cambia de tama�o las im�genes de renderizado, tanto las de color
		/// como las de profundidad.
		/// @param targetSize Nueva resoluci�n de las im�genes de renderizado.
		/// 
		/// @pre El render target debe haber sido correctamente creado mediante
		/// RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// 
		/// @warning Esta funci�n destruye las im�genes anteriores, por lo que si una instancia de material
		/// referenciaba estas im�genes, se deber� actualizar con las nuevas im�genes.
		void Resize(const Vector2ui& targetSize);


		/// @brief Devuelve la imagen de renderizado en los �ndices dados.
		/// @param colorImageIndex �ndice de la imagen objetivo de color.
		/// @param resourceIndex �ndice del frame.
		/// @return Puntero no nulo a la imagen.
		/// 
		/// @pre Este render target debe de haberse inicializado previamente con 
		/// RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// @pre colorImageIndex debe apuntar a un target existente (colorImageIndex < RenderTarget::GetNumColorTargets).
		/// @pre resourceIndex debe estar entre 0 y NUM_RESOURCES_IN_FLIGHT 
		/// (0 <= resourceIndex < NUM_RESOURCES_IN_FLIGHT).
		/// 
		/// @throws InvalidArgumentException si se incumple alguna precondici�n.
		GpuImage* GetColorImage(UIndex32 colorImageIndex, UIndex32 resourceIndex) const;

		/// @brief Devuelve la imagen de renderizado principal (�ndice 0).
		/// @param resourceIndex �ndice del frame.
		/// @return Puntero no nulo a la imagen.
		/// 
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// 
		/// @throws std::runtime_error si se incumple la precondici�n.
		GpuImage* GetMainColorImage(UIndex32 resourceIndex) const;

		/// @brief Devuelve la imagen de profundidad.
		/// @param resourceIndex �ndice del frame.
		/// @return Puntero no nulo a la imagen.
		/// 
		/// @pre Debe de haberse inicializado con RenderTarget::Create o RenderTarget::CreateAsFinal.
		/// @pre resourceIndex debe estar entre 0 y NUM_RESOURCES_IN_FLIGHT 
		/// (0 <= resourceIndex < NUM_RESOURCES_IN_FLIGHT).
		/// 
		/// @throws InvalidArgumentException Si se incumple alguna de las precondiciones.
		GpuImage* GetDepthImage(UIndex32 resourceIndex) const;


		/// @brief N�mero de targets de color.
		/// @return N�mero de im�genes objetivo de color.
		/// 
		/// @remark Ser� 0 si no se ha inicializado.
		/// @remark Ser� al menos 1 si se ha inicializado.
		USize32 GetNumColorTargets() const;

		/// @brief Devuelve la resoluci�n de todas las im�genes, en p�xeles.
		/// @return Resoluci�n del render target.
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
