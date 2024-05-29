#pragma once

#include "ApiCall.h"
#include "MaterialInstance.h"
#include "DynamicArray.hpp"
#include "Vector4.hpp"
#include "Color.hpp"
#include "TextureCoordinates.h"

#include "GameObject.h"

namespace OSK::ASSETS {
	class Texture;
	class Font;
	class FontInstance;
}

namespace OSK::ECS { 
	class Transform2D;
	class CameraComponent2D;
}

namespace OSK::GRAPHICS {

	class ICommandList;
	class GpuImage;
	class Material;
	class Sprite;

	/// <summary>
	/// Clase que simplifica el proceso de renderizado de imágenes 2D
	/// sin tener que usar el sistema RenderSystem2D, lo cual te da un
	/// mayor control sobre el orden en el que se renderizan los sprites.
	/// 
	/// @note Antes de usar los comandos de renderizado, se debe llamar a
	/// SpriteRenderer::SetCommandList y SpriteRenderer::Begin.
	/// @note Despues de finalizar el renderizado, se debe llamar a SpriteRenderer::End.
	/// 
	/// @warning No se deben ejecutar otros comandos de renderizado desde
	/// que se llama a SpriteRenderer::Begin hasta que se llama a 
	/// SpriteRenderer::End.
	/// </summary>
	/// 
	/// \todo DrawString con acciones si no cabe en la pantalla (nueva línea, mover texto, etc).
	/// \todo Anchors
	class OSKAPI_CALL SpriteRenderer {

	public:

		/// @brief Establece la cola de comandos sobre la que se grabarán
		/// los comandos de renderizado.
		/// @param commandList Lista de comandos gráficos.
		void SetCommandList(ICommandList* commandList) noexcept;


		/// @brief Establece el material que se usará sobre los siguientes sprites.
		/// @param material Material a usar.
		void SetMaterial(const Material& material);

		/// @brief Establece un material slot que se usará sobre los siguientes sprites.
		/// @param material Material slot a usar.
		void SetMaterialSlot(const IMaterialSlot& materialSlot);


		/// @brief Establece la cámara 2D con la que se renderizarán los próximos sprites.
		/// @param camera Cámara 2D.
		/// @param cameraTransform Transform de la cámara.
		void SetCamera(const ECS::CameraComponent2D& camera, const ECS::Transform2D& cameraTransform);

		/// @brief Establece la cámara 2D con la que se renderizarán los próximos sprites.
		/// @param gameObject ID de un objeto con una cámara 2D.
		/// @pre @p gameObject debe tener un componente del tipo CameraComponent2D.
		/// @pre @p gameObject debe tener un componente del tipo Transform2D.
		void SetCamera(ECS::GameObjectIndex gameObject);

		
		/// @brief Inicializa el renderizado 2D, estableciendo los recursos necesarios.
		/// @pre Se debe haber establecido correctamente la lista de comandos.
		/// @throws InvalidObjectStateException Si se incumple la precondición.
		void Begin();


		/// @brief Renderiza un sprite 2D.
		/// @param sprite Sprite que se va a renderizar.
		/// @param transform Transformación 2D del sprite.
		/// @param color Color del sprite (se usará en vez del color de @p sprite).
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		/// 
		/// @throws InvalidObjectStateException Si se incumple alguna precondición.
		void Draw(
			const Sprite& sprite, 
			const ECS::Transform2D& transform, 
			const Color& color);

		/// Renderiza un sprite 2D.
		/// @param sprite Sprite que se va a renderizar.
		/// @param texCoords Coordenadas de texturas (se usará en vez de las coordenadas de @p sprite).
		/// @param transform Transform del sprite.
		/// @param color Color del sprite (se usará en vez del color de @p sprite).
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		void Draw(
			const Sprite& sprite, 
			const TextureCoordinates2D& texCoords, 
			const ECS::Transform2D& transform, 
			const Color& color);

		/// @brief Renderiza un sprite.
		/// @param sprite Sprite que se va a renderizar.
		/// @param texCoords Coordenadas de texturas (se usará en vez de las coordenadas de @p sprite).
		/// @param position Posición del sprite en el mundo 2D.
		/// @param size Tamaño del sprite en el mundo 2D.
		/// @param rotation Rotación del sprite en el mundo 2D.
		/// @param color Color del sprite (se usará en vez del color de @p sprite).
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		void Draw(
			const Sprite& sprite,
			const TextureCoordinates2D& texCoords, 
			const Vector2f position,  
			const Vector2f size, 
			float rotation, 
			const Color& color);

		/// @brief Renderiza un sprite 2D.
		/// @param sprite Sprite a renderizar.
		/// @param transform Transform del sprite.
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		void Draw(
			const Sprite& sprite, 
			const ECS::Transform2D& transform);

		/// @brief Renderiza un sprite 2D.
		/// @param sprite Sprite que se va a renderizar.
		/// @param texCoords Coordenadas de texturas (se usará en vez de las coordenadas de @p sprite).
		/// @param transform Transform del sprite.
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		void Draw(
			const Sprite& sprite, 
			const TextureCoordinates2D& texCoords, 
			const ECS::Transform2D& transform);

		/// @brief Renderiza un sprite.
		/// @param sprite Sprite que se va a renderizar.
		/// @param texCoords Coordenadas de texturas (se usará en vez de las coordenadas de @p sprite).
		/// @param position Posición del sprite en el mundo 2D.
		/// @param size Tamaño del sprite en el mundo 2D.
		/// @param rotation Rotación del sprite en el mundo 2D.
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		void Draw(
			const Sprite& sprite, 
			const TextureCoordinates2D& texCoords, 
			const Vector2f position, 
			const Vector2f size, 
			float rotation);


		/// @brief Renderiza el texto con la fuente indicada.
		/// @param font Fuente del texto.
		/// @param fontSize Tamaño de la fuente (en píxeles).
		/// @param text Texto a renderizar.
		/// @param transform Transform del texto.
		/// @param color Color del texto.
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		void DrawString(
			ASSETS::Font& font,
			USize32 fontSize,
			std::string_view text,
			const ECS::Transform2D& transform,
			const Color& color);

		/// @brief Renderiza el texto con la fuente indicada.
		/// @param font Fuente a usar.
		/// @param fontSize Tamaño de la fuente, en píxeles.
		/// @param text Texto a renderizar.
		/// @param position Posición del texto.
		/// @param size Tamaño de cada letra, multiplicador respecto al tamaño de fuente (1.0 para tamaño original).
		/// @param rotation Rotación del texto.
		/// @param color Color del texto.
		void DrawString(
			ASSETS::Font& font,
			USize32 fontSize,
			std::string_view text,
			const Vector2f position,
			const Vector2f size,
			float rotation, const
			Color& color);


		/// @brief Renderiza el texto con la fuente indicada.
		/// @param font Fuente del texto.
		/// @param fontSize Tamaño de la fuente (en píxeles).
		/// @param text Texto a renderizar.
		/// @param transform Transform del texto.
		/// @param color Color del texto.
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// @pre Se debe haber establecido un material.
		/// 
		/// @pre @p font debe tener previamente cargada una instancia con tamaño @p fontSize.
		void DrawString(
			const ASSETS::Font& font,
			USize32 fontSize,
			std::string_view text,
			const ECS::Transform2D& transform,
			const Color& color);

		/// @brief Renderiza el texto con la fuente indicada.
		/// @param font Fuente a usar.
		/// @param fontSize Tamaño de la fuente, en píxeles.
		/// @param text Texto a renderizar.
		/// @param position Posición del texto.
		/// @param size Tamaño de cada letra, multiplicador respecto al tamaño de fuente (1.0 para tamaño original).
		/// @param rotation Rotación del texto.
		/// @param color Color del texto.
		/// @pre @p font debe tener previamente cargada una instancia con tamaño @p fontSize.
		void DrawString(
			const ASSETS::Font& font,
			USize32 fontSize,
			std::string_view text,
			const Vector2f position,
			const Vector2f size,
			float rotation, const
			Color& color);

		/// @brief Finaliza el renderizado 2D.
		void End() noexcept;

	private:

		void DrawString(
			const ASSETS::FontInstance& font,
			USize32 fontSize,
			std::string_view text,
			const ECS::Transform2D& transform,
			const Color& color);

		ICommandList* targetCommandList = nullptr;

		bool isStarted = false;

		glm::mat4 currentCameraMatrix = glm::mat4(1.0f);

		const IGpuImageView* previousImage = nullptr;
		const Material* currentlyBoundMaterial = nullptr;

	};

}
