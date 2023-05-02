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
	/// @todo Establecer unidad de tama�o independiente de la resoluci�n.
	class OSKAPI_CALL IElement {

	protected:

		/// @brief Establece el tama�o de la interfaz.
		/// @param size 
		IElement(const Vector2f& size);

	public:

		OSK_DEFINE_AS(IElement);

		/// @brief Renderiza el elemento.
		/// @param renderer Renderizador de sprites.
		/// @param parentPosition Posici�n del elemento padre.
		virtual void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition = 0.0f) const;

		/// @brief Actualiza el estado o cualquier otra caracter�stica del
		/// elemento de acuerdo a la posici�n del cursor (rat�n).
		/// @param cursorPosition Posici�n del cursor.
		/// @param isPressed True si se est� pulsando el cursor.
		/// @param parentPosition Posici�n del contenedor padre.
		virtual void UpdateByCursor(Vector2f cursorPosition, bool isPressed, Vector2f parentPosition = 0.0f);

	public:

		/// @brief Establece el padding (m�rgenes interiores).
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo y derecho.
		/// Y -> padding superior e inferior.
		void SetPadding(const Vector2f& padding);

		/// @brief Establece el padding (m�rgenes interiores).
		/// @param padding 
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo.
		/// Y -> padding superior.
		/// Z -> padding derecho.
		/// W -> padding inferior.
		void SetPadding(const Vector4f& padding);


		/// @brief Establece los m�rgenes externos.
		/// @param margin M�rgenes, tal que:
		/// X -> margen izquierdo y derecho.
		/// Y -> margen superior e inferior.
		void SetMargin(const Vector2f& margin);

		/// @brief Establece los m�rgenes externos.
		/// @param margin M�rgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		void SetMargin(const Vector4f& margin);


		/// @brief Establece la posici�n relativa de este elemento respecto a su contenedor.
		/// @param relativePosition Posici�n relativa.
		virtual void _SetRelativePosition(const Vector2f& relativePosition);

		/// @return Posici�n relativa respecto a su contenedor.
		Vector2f GetRelativePosition() const;


		/// @brief Cambia el tama�o del elemento, sin modifciar contenedores
		/// ni elementos hijos.
		/// @param newSize Nuevo tama�o.
		/// 
		/// @warning No modifica ni la posici�n ni el tama�o de elementos padre,
		/// hijos o hermanos.
		void SetSize(Vector2f newSize);

		/// @return Tama�o del elemento.
		Vector2f GetSize() const;

		/// @return Tama�o del contenido del elemento (tama�o - padding).
		Vector2f GetContentSize() const;

		/// @return Posici�n relativa del contenido (posici�n - padding).
		Vector2f GetContentTopLeftPosition() const;


		/// @return Rect�ngulo que ocupa el elemento, tal que:
		/// X, Y -> posici�n.
		/// Z -> ancho.
		/// W -> alto.
		Vector4f GetRectangle(Vector2f parentPosition) const;

		/// @brief Establece el anchor del elemento.
		/// No cambiar� su posici�n respecto al contenedor padre.
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

		/// @return M�rgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		Vector4f GetMarging() const;

		/// @return Padding, tal que:
		/// X -> padding izquierdo + padding derecho.
		/// Y -> padding superior + padding inferior.
		Vector2f GetPadding2D() const;

		/// @return M�rgenes, tal que:
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
