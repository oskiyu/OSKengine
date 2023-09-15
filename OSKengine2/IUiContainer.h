#pragma once

#include "UiElement.h"
#include "SharedPtr.hpp"
#include "HashMap.hpp"
#include "UiAnchor.h"

#include "Vector2.hpp"

namespace OSK::UI {

	/// @brief Interfaz com�n para los contenedores de UI.
	/// Un contenedor de UI puede contener distintos elementos UI
	/// dentro. Para cada layout se deber� sobreescribir esta clase.
	class OSKAPI_CALL IContainer : public IElement {

	protected:

		explicit IContainer(const Vector2f& size) : IElement(size) {  }

	public:

		void SetSize(Vector2f newSize) override;

	public:

		inline GRAPHICS::Sprite& GetSprite() { return sprite; }
		inline const GRAPHICS::Sprite& GetSprite() const { return sprite; }

		/// @brief Ajusta el tama�o del contenedor, para que sea el justo y
		/// necesario para que quepan todos los elementos hijos.
		void AdjustSizeToChildren();

		/// @brief A�ade un elemento al contenedor. 
		/// Modificar� su posici�n dependiendo del layout implementado.
		/// 
		/// @param key Identificador del elemento. Debe ser �nico dentro de este contenedor.
		/// @param child Referencia compartida al elemento.
		/// 
		/// @pre El elemento @p child no debe estar ya a�adido a ning�n contenedor.
		/// @pre Este contenedor no debe tener ning�n elemento con el identificador @p key.
		void AddChild(const std::string& key, SharedPtr<IElement> child);

		/// @brief Reestructura todos los hijos del contenedor.
		void Rebuild();


		/// @brief Renderiza todos los elementos contenidos.
		void Render(GRAPHICS::SpriteRenderer* renderer, Vector2f parentPosition) const override;

		bool UpdateByCursor(Vector2f cursorPosition, bool isPressed, Vector2f parentPosition) override;

		/// @param name Nombre del elemento hijo.
		/// @return Puntero al elemento indicado.
		/// @pre Debe haberse introducido un elemento con el nombre dado.
		IElement* GetChild(const std::string_view name) const;

	protected:

		/// @brief Resetea el layout, de tal manera que los siguientes
		/// elementos colocados se coloquen en el inicio del layout.
		virtual void ResetLayout() = 0;

		/// @brief Coloca un hijo en la posici�n deseada, seg�n el layout.
		/// @param child Hijo a cambiar de posici�n.
		virtual void EmplaceChild(IElement* child) = 0;

	protected:

		GRAPHICS::Sprite sprite{};

		DynamicArray<SharedPtr<IElement>> children;
		std::unordered_map<std::string, IElement*> childrenTable{};

	};

}
