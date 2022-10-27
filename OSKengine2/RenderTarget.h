#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IGpuImage.h"
#include "Vector2.hpp"

#include "Sprite.h"
#include "Transform2D.h"
#include "RenderpassType.h"
#include "GpuImageUsage.h"
#include "GpuImageSamplerDesc.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Im�genes de renderizado contenidas en cada RenderTarget.
	/// Permiten el renderizado de frames mientras se procesa el
	/// juego en la CPU.
	/// </summary>
	constexpr TSize NUM_RENDER_TARGET_IMAGES = 3;

	/// <summary>
	/// Representa una textura a la que se puede renderizar.
	/// 
	/// Incluye:
	///		- Im�genes de renderizado.
	///		- Sprite
	/// </summary>
	class OSKAPI_CALL RenderTarget {

	public:

		/// <summary>
		/// Crea un render target.
		/// 
		/// Contiene varias im�genes de renderizado, para permitir un renderizado
		/// interlazado con el procesado en la CPU.
		/// </summary>
		/// <param name="targetSize">Tama�o de las im�genes de renderizado.</param>
		/// <param name="colorFormat">Formato de las im�genes de renderizado.</param>
		/// <param name="depthFormat">Formato de las im�genes de profundidad.</param>
		/// 
		/// @note La resoluci�n final de las im�genes tambi�n es afectada por 
		/// la escala de resoluci�n del render target (RenderTarget::SetResolutionScale).
		void Create(const Vector2ui& targetSize, Format colorFormat, Format depthFormat);

		/// <summary>
		/// A�ade un nuevo color target.
		/// </summary>
		/// <param name="colorFormat">Formato de las im�genes de renderizado.</param>
		/// 
		/// @pre Debe haberse creado el rendertarget mediante RenderTarget::Create.
		void AddColorTarget(Format colorFormat, GpuImageUsage usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED, GpuImageSamplerDesc sampler = {});


		/// <summary>
		/// Cambia de tama�o las im�genes de renderizado, tanto las de color
		/// como las de profundidad.
		/// </summary>
		/// <param name="targetSize">Nuevo tama�o de las im�genes de renderizado.</param>
		/// 
		/// @note La resoluci�n final de las im�genes tambi�n es afectada por 
		/// la escala de resoluci�n del render target (RenderTarget::SetResolutionScale).
		void Resize(const Vector2ui& targetSize);

		/// <summary>
		/// Devuelve la imagen de renderizado con el �ndice dado.
		/// </summary>
		/// 
		/// @pre El �ndice debe estar entre 0 y 2, ambos incluidos.
		/// 
		/// @throws std::runtime_exception si no se cumple las precondiciones.
		GpuImage* GetMainTargetImage(TSize index) const;

		/// <summary>
		/// Devuelve las im�genes de renderizado con el �ndice dado.
		/// El �ndice es el �ndice del frame.
		/// </summary>
		/// 
		/// @pre El �ndice debe estar entre 0 y 2, ambos incluidos.
		/// 
		/// @throws std::runtime_exception si no se cumple las precondiciones.
		DynamicArray<GpuImage*> GetTargetImages(TSize index) const;

		/// <summary>
		/// Devuelve la imagen de profundidad con el �ndice dado.
		/// </summary>
		/// 
		/// @pre El �ndice debe estar entre 0 y 2, ambos incluidos.
		/// 
		/// @throws std::runtime_exception si no se cumple las precondiciones.
		GpuImage* GetDepthImage(TSize index) const;

		/// <summary>
		/// Devuelve el sprite de la imagen de renderizado.
		/// </summary>
		/// 
		/// @warning No se puede renderizar con el material 2D por defecto.
		/// @warning Se debe renderizar con el material 2D de render target.
		const Sprite& GetSprite() const;

		/// <summary>
		/// Devuelve el transform del sprite.
		/// 
		/// El transform est� configurado para que su renderizado ocupe toda la pantalla.
		/// </summary>
		const ECS::Transform2D& GetSpriteTransform() const;

		/// <summary>
		/// Devuelve el tama�o del render target.
		/// </summary>
		/// 
		/// @note Si no se ha inicializado, ser� 0.
		Vector2ui GetSize() const;

		/// <summary>
		/// Devuelve el tama�o del render target.
		/// </summary>
		/// 
		/// @note Si no se ha inicializado, ser� 0.
		Vector2ui GetOriginalSize() const;

		/// <summary>
		/// Devuelve el formato del render target.
		/// </summary>
		Format GetColorFormat() const;

		/// <summary>
		/// Devuelve el formato de las im�genes de profundidad.
		/// </summary>
		Format GetDepthFormat() const;

		/// <summary>
		/// Devuelve el tipo de render target (intermedio para ser usado
		/// con el sprite, o final para ser renderizado en la propia pantalla).
		/// </summary>
		/// 
		/// @note Por defecto, es RenderpassType::INTERMEDIATE.
		RenderpassType GetRenderTargetType() const;

		/// <summary>
		/// Establece el tipo de render target.
		/// </summary>
		/// 
		/// @note Debe ser llamado antes de RenderTarget::Create.
		/// @note Si es llamado despu�s de RenderTarget::Create, no tendr� efecto hasta que se
		/// llame a RenderTarget::Resize.
		/// 
		/// @note �nicamente se debe establecer como RenderpassType::FINAL si las im�genes
		/// enlazadas al render target son las im�genes del propio swapchain.
		void SetRenderTargetType(RenderpassType type);

		/// <summary>
		/// Establece un multiplicador a la resoluci�n
		/// del render target.
		/// </summary>
		/// 
		/// @pre scale debe ser > 0.
		/// 
		/// @note Debe ser llamado antes de RenderTarget::Create.
		/// @note Si es llamado despu�s de RenderTarget::Create, no tendr� efecto hasta que se
		/// llame a RenderTarget::Resize.
		void SetResolutionScale(float scale);

		/// <summary>
		/// Establece el uso que se le va a dar a las im�genes de renderizado.
		/// </summary>
		/// <param name="usage">Uso que se le va a dar.</param>
		/// 
		/// @note Debe ser llamado antes de RenderTarget::Create.
		/// @note Si es llamado despu�s de RenderTarget::Create, no tendr� efecto hasta que se
		/// llame a RenderTarget::Resize.
		/// 
		/// @note Por defecto, ser� GpuImageUsage::COLOR | GpuImageUsage::SAMPLED.
		/// @note Tendr� uso GpuImageUsage::COLOR | GpuImageUsage::SAMPLED | usage.
		void SetTargetImageUsage(GpuImageUsage usage);

		/// <summary>
		/// Establece el uso que se le va a dar a las im�genes de renderizado.
		/// </summary>
		/// <param name="usage">Uso que se le va a dar.</param>
		/// 
		/// @note Debe ser llamado antes de RenderTarget::Create.
		/// @note Si es llamado despu�s de RenderTarget::Create, no tendr� efecto hasta que se
		/// llame a RenderTarget::Resize.
		/// 
		/// @note Por defecto, ser� GpuImageUsage::DEPTH_STENCIL.
		/// @note Tendr� uso GpuImageUsage::DEPTH_STENCIL | usage.
		void SetDepthImageUsage(GpuImageUsage usage);


		/// <summary>
		/// Establece el sampler para las im�genes de renderizado, que determina c�mo se accede a la textura desde
		/// los shaders.
		/// </summary>
		void SetColorImageSampler(const GpuImageSamplerDesc& sampler);

		/// <summary>
		/// Establece el sampler para las im�genes de profundidad, que determina c�mo se accede a la textura desde
		/// los shaders.
		/// </summary>
		void SetDepthImageSampler(const GpuImageSamplerDesc& sampler);

		/// <summary>
		/// Especifica cual de todas las im�genes de target del render target
		/// ser� mostrada como output al usarlo como sprite.
		/// </summary>
		/// <param name="targetImage">�ndice del target �mage, seg�n el orden en el que se hayan a�adido. </param>
		/// 
		/// @pre targetImage debe ser menor al n�mero de targets registrados en el render target.
		void SetSpriteTargetImage(TIndex targetImage);

		/// <summary>
		/// Devuelve el n�mero de im�genes de color del render target.
		/// </summary>
		TSize GetNumColorTargets() const;

		void SetName(const std::string& name);

	private:

		void CreateDepthImages(Format format);
		void UpdateSpriteImages();

		void RecreateTargetImages();
		void RecreateDepthImages();

		Vector2ui size = 0;
		float resolutionScale = 1.0f;

		GpuImageUsage targetUsage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED;
		GpuImageUsage depthUsage = GpuImageUsage::DEPTH_STENCIL;

		DynamicArray<UniquePtr<GpuImage>> targetImages[NUM_RENDER_TARGET_IMAGES]; // Puede haber m�s de un target.
		UniquePtr<GpuImage> depthImages[NUM_RENDER_TARGET_IMAGES];

		Sprite targetSprite;
		ECS::Transform2D spriteTransform{ ECS::EMPTY_GAME_OBJECT };

		GpuImageSamplerDesc colorSampler{};
		GpuImageSamplerDesc depthSampler{};

		RenderpassType targetType = RenderpassType::INTERMEDIATE;

		TIndex targetSpriteImage = 0;

	};

}
