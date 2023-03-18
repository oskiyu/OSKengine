#pragma once

#include "OSKmacros.h"
#include "LinkedList.hpp"
#include "UniquePtr.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"

#include "Sprite.h"
#include "Color.hpp"
#include "Vector4.hpp"

#include <glm/glm.hpp>
#include <string>

namespace OSK::UI {

	/// <summary>
	/// Elemento base de una interfaz de usuario.
	/// </summary>
	class OSKAPI_CALL UiElement {

	public:

		/// <summary>
		/// Añade un elemento ahijado.
		/// </summary>
		/// 
		/// @note Un elemento solo se puede añadir como hijo a un único elemento padre.
		void AddChildElement(OwnedPtr<UiElement> element);

		/// <summary>
		/// Añade un elemento ahijado.
		/// Se puede especificar un nombre que lo identifique,
		/// para poder acceder a él más adelante.
		/// </summary>
		/// 
		/// @note Un elemento solo se puede añadir como hijo a un único elemento padre.
		/// 
		/// @note Si ya hay un elemento con ese nombre, él nombre ahora identificará
		/// al nuevo elemento, pero no se eliminará el anterior.
		void AddChildElement(OwnedPtr<UiElement> element, const std::string& key);

		/// <summary>
		/// Establece la posición del elemento, respecto al padre.
		/// </summary>
		void SetPosition(const Vector2f& pos);

		/// <summary>
		/// Establece el tamaño del elemento, respecto al padre.
		/// </summary>
		void SetSize(const Vector2f& size);

		/// <summary>
		/// Devuelve la posición respecto al padre.
		/// </summary>
		Vector2f GetPosition() const;

		/// <summary>
		/// Devuelve el tamaño respecto al padre.
		/// </summary>
		Vector2f GetSize() const;

		/// <summary>
		/// Devuelve la posición y el tamaño respecto al padre.
		/// </summary>
		Vector4f GetRectangle() const;

		/// <summary>
		/// Devuelve un elemnto ahijado, con ese nombre.
		/// </summary>
		/// 
		/// @note Únicamente busca dentro de sus hijos, y no de los
		/// hijos de los hijos, etc...
		UiElement* GetChildElement(const std::string& name);

		const LinkedList<UniquePtr<UiElement>>& GetChildElements() const;

		Color color = Color::WHITE();
		GRAPHICS::Sprite sprite;

		glm::mat4 GetMatrix() const;

	private:

		Vector4f rectangle = { 0, 0, 1, 1 };

		LinkedList<UniquePtr<UiElement>> childElements;
		HashMap<std::string, UiElement*> childElementsTable;

	};

}
