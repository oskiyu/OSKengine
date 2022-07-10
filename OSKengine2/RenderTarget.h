#pragma once

#include "OSKmacros.h"
#include "UniquePtr.hpp"
#include "IRenderpass.h"
#include "IGpuImage.h"
#include "Vector2.hpp"

#include "Sprite.h"
#include "Transform2D.h"
#include "RenderpassType.h"

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
	///		- Imagen de renderizado.
	///		- Renderpass.
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
		/// @note �nicamente se debe establecer como RenderpassType::FINAL si las im�genes
		/// enlazadas al render target son las im�genes del propio swapchain.
		void SetRenderTargetType(RenderpassType type);

		/// <summary>
		/// Establece un multiplicador a la resoluci�n
		/// del render target.
		/// </summary>
		/// 
		/// @pre scale debe ser > 0.
		void SetResolutionScale(float scale);

	private:

		void CreateTargetImages();
		void CreateDepthImages();
		void UpdateSpriteImages();

		Vector2ui size = 0;
		float resolutionScale = 1.0f;
		Format colorFormat;
		Format depthFormat;

		DynamicArray<UniquePtr<GpuImage>> targetImages[NUM_RENDER_TARGET_IMAGES]; // Puede haber m�s de un target.
		UniquePtr<GpuImage> depthImages[NUM_RENDER_TARGET_IMAGES];

		Sprite targetSprite;
		ECS::Transform2D spriteTransform{ ECS::EMPTY_GAME_OBJECT };

		RenderpassType targetType = RenderpassType::INTERMEDIATE;

	};

}
