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
	/// Imágenes de renderizado contenidas en cada RenderTarget.
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
		/// Contiene varias imágenes de renderizado, para permitir un renderizado
		/// interlazado con el procesado en la CPU.
		/// </summary>
		/// <param name="targetSize">Tamaño de las imágenes de renderizado.</param>
		/// <param name="colorFormat">Formato de las imágenes de renderizado.</param>
		/// <param name="depthFormat">Formato de las imágenes de profundidad.</param>
		/// 
		/// @note La resolución final de las imágenes también es afectada por 
		/// la escala de resolución del render target (RenderTarget::SetResolutionScale).
		void Create(const Vector2ui& targetSize, Format colorFormat, Format depthFormat);

		/// <summary>
		/// Cambia de tamaño las imágenes de renderizado, tanto las de color
		/// como las de profundidad.
		/// </summary>
		/// <param name="targetSize">Nuevo tamaño de las imágenes de renderizado.</param>
		/// 
		/// @note La resolución final de las imágenes también es afectada por 
		/// la escala de resolución del render target (RenderTarget::SetResolutionScale).
		void Resize(const Vector2ui& targetSize);

		/// <summary>
		/// Devuelve la imagen de renderizado con el índice dado.
		/// </summary>
		/// 
		/// @pre El índice debe estar entre 0 y 2, ambos incluidos.
		/// 
		/// @throws std::runtime_exception si no se cumple las precondiciones.
		GpuImage* GetMainTargetImage(TSize index) const;

		/// <summary>
		/// Devuelve las imágenes de renderizado con el índice dado.
		/// </summary>
		/// 
		/// @pre El índice debe estar entre 0 y 2, ambos incluidos.
		/// 
		/// @throws std::runtime_exception si no se cumple las precondiciones.
		DynamicArray<GpuImage*> GetTargetImages(TSize index) const;

		/// <summary>
		/// Devuelve la imagen de profundidad con el índice dado.
		/// </summary>
		/// 
		/// @pre El índice debe estar entre 0 y 2, ambos incluidos.
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
		/// El transform está configurado para que su renderizado ocupe toda la pantalla.
		/// </summary>
		const ECS::Transform2D& GetSpriteTransform() const;

		/// <summary>
		/// Devuelve el tamaño del render target.
		/// </summary>
		/// 
		/// @note Si no se ha inicializado, será 0.
		Vector2ui GetSize() const;

		/// <summary>
		/// Devuelve el tamaño del render target.
		/// </summary>
		/// 
		/// @note Si no se ha inicializado, será 0.
		Vector2ui GetOriginalSize() const;

		/// <summary>
		/// Devuelve el formato del render target.
		/// </summary>
		Format GetColorFormat() const;

		/// <summary>
		/// Devuelve el formato de las imágenes de profundidad.
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
		/// @note Únicamente se debe establecer como RenderpassType::FINAL si las imágenes
		/// enlazadas al render target son las imágenes del propio swapchain.
		void SetRenderTargetType(RenderpassType type);

		/// <summary>
		/// Establece un multiplicador a la resolución
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

		DynamicArray<UniquePtr<GpuImage>> targetImages[NUM_RENDER_TARGET_IMAGES]; // Puede haber más de un target.
		UniquePtr<GpuImage> depthImages[NUM_RENDER_TARGET_IMAGES];

		Sprite targetSprite;
		ECS::Transform2D spriteTransform{ ECS::EMPTY_GAME_OBJECT };

		RenderpassType targetType = RenderpassType::INTERMEDIATE;

	};

}
