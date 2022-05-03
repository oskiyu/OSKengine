#pragma once

#include "OSKmacros.h"
#include "MaterialInstance.h"
#include "DynamicArray.hpp"
#include "Vector4.hpp"
#include "Color.hpp"
#include "TextureCoordinates.h"

namespace OSK::ASSETS {
	class Texture;
	class Font;
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
	/// Clase que simplifica el proceso de renderizado de im�genes 2D
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
	/// \todo DrawString con acciones si no cabe en la pantalla (nueva l�nea, mover texto, etc).
	/// \todo Anchors
	class OSKAPI_CALL SpriteRenderer {

	public:

		/// <summary>
		/// Establece la cola de comandos sobre la que se grabar�n
		/// los comandos de renderizado.
		/// </summary>
		void SetCommandList(ICommandList* commandList) noexcept;


		/// <summary>
		/// Inicializa el renderizado 2D, estableciendo la c�mara
		/// 2D que se usar� durante el renderizado.
		/// </summary>
		/// 
		/// @pre Si esta no es la primera vez que se ejecuta,
		/// se debe haber llamado a End antes de llamar a Begin.
		/// 
		/// @throws std::runtime_error Si se incumple la segunda precondici�n.
		void Begin();

		/// <summary>
		/// Renderiza un sprite 2D.
		/// </summary>
		/// <param name="sprite">Sprite que se va a renderizar.</param>
		/// <param name="transform">Transformaci�n 2D que se aplicar� al 
		/// renderizar el sprite.</param>
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// 
		/// @throws std::runtime_error Si se incumple alguna de las precondiciones.
		void Draw(const Sprite& sprite, const ECS::Transform2D& transform, const Color& color);

		/// <summary>
		/// Renderiza un sprite 2D.
		/// </summary>
		/// <param name="sprite">Sprite que se va a renderizar.</param>
		/// <param name="transform">Transformaci�n 2D que se aplicar� al 
		/// renderizar el sprite.</param>
		/// <param name="texCoords">Coordenadas de texturas.</param>
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// 
		/// @throws std::runtime_error Si se incumple alguna de las precondiciones.
		void Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const ECS::Transform2D& transform, const Color& color);

		/// <summary>
		/// Renderiza una imagen 2D.
		/// </summary>
		/// <param name="sprite">Sprite a renderizar.</param>
		/// <param name="texCoords">Coordenadas de texturas del sprite.</param>
		/// <param name="position">Posici�n, en p�xeles, en el mundo 2D.</param>
		/// <param name="size">Tama�o, en p�xeles.</param>
		/// <param name="rotation">Rotaci�n. En grados y en sentido horario.</param>
		/// <param name="color">Tinte de la imagen.</param>
		/// 
		/// @throws std::runtime_error Si se incumple alguna de las precondiciones.
		void Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const Vector2f position, const Vector2f size, float rotation, const Color& color);

		/// <summary>
		/// Renderiza un sprite 2D.
		/// </summary>
		/// <param name="sprite">Sprite que se va a renderizar.</param>
		/// <param name="transform">Transformaci�n 2D que se aplicar� al 
		/// renderizar el sprite.</param>
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// 
		/// @throws std::runtime_error Si se incumple alguna de las precondiciones.
		void Draw(const Sprite& sprite, const ECS::Transform2D& transform);

		/// <summary>
		/// Renderiza un sprite 2D.
		/// </summary>
		/// <param name="sprite">Sprite que se va a renderizar.</param>
		/// <param name="transform">Transformaci�n 2D que se aplicar� al 
		/// renderizar el sprite.</param>
		/// <param name="texCoords">Coordenadas de texturas.</param>
		/// 
		/// @pre La cola de comandos debe estar iniciada.
		/// @pre La cola de comandos debe tener asociado un renderpass.
		/// 
		/// @throws std::runtime_error Si se incumple alguna de las precondiciones.
		void Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const ECS::Transform2D& transform);

		/// <summary>
		/// Renderiza una imagen 2D.
		/// </summary>
		/// <param name="sprite">Sprite a renderizar.</param>
		/// <param name="texCoords">Coordenadas de texturas del sprite.</param>
		/// <param name="position">Posici�n, en p�xeles, en el mundo 2D.</param>
		/// <param name="size">Tama�o, en p�xeles.</param>
		/// <param name="rotation">Rotaci�n. En grados y en sentido horario.</param>
		/// 
		/// @throws std::runtime_error Si se incumple alguna de las precondiciones.
		void Draw(const Sprite& sprite, const TextureCoordinates2D& texCoords, const Vector2f position, const Vector2f size, float rotation);

		/// <summary>
		/// Renderiza el texto con la fuente indicada.
		/// </summary>
		/// <param name="font">Fuente del texto.</param>
		/// <param name="fontSize">Tama�o de la fuente, en p�xeles.</param>
		/// <param name="text">Texto a renderizar.</param>
		/// <param name="transform">Transform del texto.</param>
		/// <param name="color">Color del texto.</param>
		/// 
		/// @note La escala del transform representar� la proporci�n del tama�o de cada
		/// letra respecto al tama�o de la fuente (= 1 para el tama�o original de la fuente).
		/// 
		/// @bug La rotaci�n del transform no se aplica correctamente.
		void DrawString(ASSETS::Font& font, TSize fontSize, const std::string& text, const ECS::Transform2D& transform, const Color& color);

		/// <summary>
		/// Renderiza el texto con la fuente indicada.
		/// </summary>
		/// <param name="font">Fuente del texto.</param>
		/// <param name="fontSize">Tama�o de la fuente, en p�xeles.</param>
		/// <param name="text">Texto a renderizar.</param>
		/// <param name="position">Posici�n de inicio (esquina inferior izquierda).</param>
		/// <param name="size">Tama�o de cada letra, multiplicador respecto al tama�o de fuente.</param>
		/// <param name="rotation">Rotaci�n de cada letra.</param>
		/// <param name="color">Color del texto.</param>
		/// 
		/// @bug La rotaci�n no se aplica correctamente.
		void DrawString(ASSETS::Font& font, TSize fontSize, const std::string& text, const Vector2f position, const Vector2f size, float rotation, const Color& color);

		/// <summary>
		/// Finaliza el renderizado 2D.
		/// </summary>
		void End() noexcept;

	private:

		ICommandList* targetCommandList = nullptr;

		bool isStarted = false;
		MaterialInstance* previousMaterialInstance = nullptr;
		const GpuImage* previousImage = nullptr;
		const Material* lastBoundMaterial = nullptr;

	};

}
