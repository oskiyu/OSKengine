#pragma once

#include "OSKmacros.h"
#include "LinkedList.hpp"
#include "UniquePtr.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "UiAnchor.h"

#include "Sprite.h"
#include "Color.hpp"
#include "Vector4.hpp"

#include <glm/glm.hpp>
#include <string>

namespace OSK::GRAPHICS {
	class SpriteRenderer;
	class Sprite;
}

namespace OSK::UI {

	/// @brief Clase base para todos los elementos de interfaz de usuario.
	/// @todo Establecer unidad de tamaño independiente de la resolución.
	class OSKAPI_CALL IElement {

	protected:

		/// @brief Establece el tamaño de la interfaz.
		/// @param size 
		IElement(const Vector2f& size);

	public:

		OSK_DEFINE_AS(IElement);

		/// @brief Renderiza el elemento.
		/// @param renderer Renderizador de sprites.
		/// @param parentPosition Posición del elemento padre.
		virtual void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition = 0.0f) const;

		/// @brief Actualiza el estado o cualquier otra característica del
		/// elemento de acuerdo a la posición del cursor (ratón).
		/// @param cursorPosition Posición del cursor.
		/// @param isPressed True si se está pulsando el cursor.
		/// @param parentPosition Posición del contenedor padre.
		virtual void UpdateByCursor(Vector2f cursorPosition, bool isPressed, Vector2f parentPosition = 0.0f);

	public:

		/// @brief Establece el padding (márgenes interiores).
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo y derecho.
		/// Y -> padding superior e inferior.
		void SetPadding(const Vector2f& padding);

		/// @brief Establece el padding (márgenes interiores).
		/// @param padding 
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo.
		/// Y -> padding superior.
		/// Z -> padding derecho.
		/// W -> padding inferior.
		void SetPadding(const Vector4f& padding);


		/// @brief Establece los márgenes externos.
		/// @param margin Márgenes, tal que:
		/// X -> margen izquierdo y derecho.
		/// Y -> margen superior e inferior.
		void SetMargin(const Vector2f& margin);

		/// @brief Establece los márgenes externos.
		/// @param margin Márgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		void SetMargin(const Vector4f& margin);


		/// @brief Establece la posición relativa de este elemento respecto a su contenedor.
		/// @param relativePosition Posición relativa.
		virtual void _SetRelativePosition(const Vector2f& relativePosition);

		/// @return Posición relativa respecto a su contenedor.
		Vector2f GetRelativePosition() const;


		/// @brief Cambia el tamaño del elemento, sin modifciar contenedores
		/// ni elementos hijos.
		/// @param newSize Nuevo tamaño.
		/// 
		/// @warning No modifica ni la posición ni el tamaño de elementos padre,
		/// hijos o hermanos.
		void SetSize(Vector2f newSize);

		/// @return Tamaño del elemento.
		Vector2f GetSize() const;

		/// @return Tamaño del contenido del elemento (tamaño - padding).
		Vector2f GetContentSize() const;

		/// @return Posición relativa del contenido (posición - padding).
		Vector2f GetContentTopLeftPosition() const;


		/// @return Rectángulo que ocupa el elemento, tal que:
		/// X, Y -> posición.
		/// Z -> ancho.
		/// W -> alto.
		Vector4f GetRectangle(Vector2f parentPosition) const;

		/// @brief Establece el anchor del elemento.
		/// No cambiará su posición respecto al contenedor padre.
		/// @param anchor Nuevo anchor.
		void SetAnchor(Anchor anchor);

		/// @return Anchor actual del elemento.
		inline Anchor GetAnchor() const { return anchor; }

		/// @return Padding, tal que:
		/// X -> padding izquierdo.
		/// Y -> padding superior.
		/// Z -> padding derecho.
		/// W -> padding inferior.
		Vector4f GetPadding() const;

		/// @return Márgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		Vector4f GetMarging() const;

		/// @return Padding, tal que:
		/// X -> padding izquierdo + padding derecho.
		/// Y -> padding superior + padding inferior.
		Vector2f GetPadding2D() const;

		/// @return Márgenes, tal que:
		/// X -> margen izquierdo + margen derecho.
		/// Y -> margen superior + margen inferior.
		Vector2f GetMarging2D() const;

	protected:

		Vector2f size = 0;

	private:

		Anchor anchor = Anchor::DEFAULT;

		Vector2f relativePosition = 0;

		Vector4f padding = 0;
		Vector4f margin = 2.0f;

	};

}
