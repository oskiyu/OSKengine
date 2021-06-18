#pragma once

#include "OSKmacros.h"
#include "Vector2.hpp"
#include "Vector4.hpp"

#include <optional>
#include <functional>

#include "Sprite.h"

#include "UiAnchor.h"
#include "UiLayout.h"

namespace OSK{

	class ContentManager;
	class RenderAPI;
	class SpriteBatch;
	class Font;

	/// <summary>
	/// Un elemento de interfaz de usuario.
	/// </summary>
	class OSKAPI_CALL UiElement {

	public:

		/// <summary>
		/// Establece la posici�n del elemento.
		/// Respecto al punto de anclaje.
		/// </summary>
		void SetPositionFromAnchor(const Vector2f& newPosition);

		/// <summary>
		/// Establece la posici�n del anclaje.
		/// </summary>
		void SetAnchorPosition(const Vector2f& newAnchor);

		/// <summary>
		/// Cambia la posici�n del elemento.
		/// Posici�n += diff.
		/// </summary>
		void AddPosition(const Vector2f& diff);

		/// <summary>
		/// Renderiza este elemento, y susu hijos.
		/// </summary>
		void Draw(SpriteBatch* spriteBatch);

		/// <summary>
		/// A�ade un elemento ahijado.
		/// </summary>
		void AddElement(UiElement* element);

		/// <summary>
		/// Cambia el tama�o del elemento, para que ocupe lo neccesario.
		/// </summary>
		void Shrink();

		/// <summary>
		/// Actualiza el estado del elemento.
		/// Para acciones OnClick...
		/// </summary>
		/// <param name="mouserRec">Rect�ngulo del rat�n.</param>
		/// <param name="click">True si se ha hecho click.</param>
		void Update(const Vector4f& mouserRec, bool click);

		/// <summary>
		/// Inicializa el sprite.
		/// </summary>
		void InitSprite(ContentManager* content, RenderAPI* renderer);

		/// <summary>
		/// Sprite a renderizar.
		/// No se renderiza si su textura es null.
		/// </summary>
		Sprite sprite;

		/// <summary>
		/// Texto a renderizar.
		/// No se renderiza si la fuente es null.
		/// </summary>
		std::string text;

		/// <summary>
		/// Fuente del texto.
		/// </summary>
		Font* fuente = nullptr;

		/// <summary>
		/// Color del texto-
		/// </summary>
		Color textColor = OSK::Color::WHITE();

		/// <summary>
		/// Elementos ahijados.
		/// </summary>
		std::vector<UiElement*> children;

		/// <summary>
		/// Layout.
		/// </summary>
		UiLayout layout = UiLayout::VERTICAL;

		/// <summary>
		/// Anchor.
		/// (S�lo cuado el padre tiene layout FIXED.
		/// </summary>
		UiAnchor anchor = UiAnchor::TOP_LEFT;

		/// <summary>
		/// Posici�n del pr�ximo elemento que se vaya a a�adir.
		/// S�lo para layouts HORIZONTAL o VERTICAL.
		/// </summary>
		Vector2f nextPos = { 0.0f };

		/// <summary>
		/// Margen interno.
		/// </summary>
		Vector2f padding = 2.0f;

		/// <summary>
		/// Margen externo.
		/// </summary>
		Vector2f marging = 2.0f;

		/// <summary>
		/// Color del sprite cuando el rat�n est� encima.
		/// </summary>
		std::optional<OSK::Color> spriteColorOnHover;

		/// <summary>
		/// Funci�n que se ejecuta cuando se hace click (una vez).
		/// </summary>
		std::function<void(UiElement* sender, const OSK::Vector2f& mousePos)> onClick = [](UiElement* sender, const OSK::Vector2f& mousePos) {};

		/// <summary>
		/// Funci�n que se ejecuta cuando se hace click (cont�nuo).
		/// </summary>
		std::function<void(UiElement* sender, const OSK::Vector2f& mousePos)> onHold = [](UiElement* sender, const OSK::Vector2f& mousePos) {};

		/// <summary>
		/// Color por defecto del sprite.
		/// </summary>
		Color originalSpireColor = Color::WHITE();

		/// <summary>
		/// Valor del slider (si implementa esta funcionalidad).
		/// </summary>
		float sliderValue = 0.0f;

		/// <summary>
		/// True si al arrastrar, tambi�n se arrastra al padre.
		/// S�lo con la funcionalidad draggable.
		/// </summary>
		bool dragMoveParent = true;

		/// <summary>
		/// Padre.
		/// </summary>
		UiElement* parent = nullptr;

		/// <summary>
		/// Si no est� activo, no se renderiza ni responde al rat�n.
		/// </summary>
		bool isActive = true;

		/// <summary>
		/// True si el elemento tiene que tener el tama�o suficiente
		/// para almacenar a todos los hijos dentro de s�.
		/// </summary>
		bool shrinkToChildren = true;

		/// <summary>
		/// True si el dropdown est� comprimido.
		/// S�lo con funcionalidad dropdown.
		/// </summary>
		bool isCompacted = false;

		/// <summary>
		/// Devuelve la posici�n real del elemento.
		/// </summary>
		Vector2f GetPosition() const;

		/// <summary>
		/// Devuelve el tama�o del elemento.
		/// </summary>
		Vector2f GetSize() const;

		/// <summary>
		/// Establece el tama�o del elemento.
		/// </summary>
		void SetSize(const Vector2f& size);

		/// <summary>
		/// Posici�n en el frame anterior.
		/// S�lo con funcionalidad draggable.
		/// </summary>
		Vector2f previousPos;

	private:

		/// <summary>
		/// Actualiza el transform del sprite.
		/// </summary>
		void UpdateSpriteMatrix();

		/// <summary>
		/// Actualiza la posici�n de los hijos.
		/// </summary>
		/// <param name="diff">Delta de posici�n.</param>
		void UpdateChildPositions(const Vector2f& diff);

		/// <summary>
		/// Actualiza la posici�n de los hijos, de acuerdo con el anchor.
		/// </summary>
		/// <param name="element">Elemento hijo.</param>
		/// <param name="init">True si </param>
		void UpdateAnchoredChildPosition(UiElement* element, bool init = false);

		/// <summary>
		/// Posici�n del ancla padre.
		/// </summary>
		Vector2f anchorPosition = 0.0f;

		/// <summary>
		/// Posici�n respecto al ancla.
		/// </summary>
		Vector2f positionDeltaFromAnchor = 0.0f;

		/// <summary>
		/// Tama�o.
		/// </summary>
		Vector2f size;

		/// <summary>
		/// True si antes estaba haciendo click.
		/// </summary>
		bool previouslyOnClick = false;

		/// <summary>
		/// Sprite del slider.
		/// S�lo con funcionalidad slider.
		/// </summary>
		Sprite sliderSprite;

	};


}