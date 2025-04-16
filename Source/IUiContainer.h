#pragma once

#include "UiElement.h"
#include "SharedPtr.hpp"
#include "HashMap.hpp"
#include "UiAnchor.h"

#include "Vector2.hpp"

namespace OSK::UI {

	/// @brief Interfaz común para los contenedores de UI.
	/// Un contenedor de UI puede contener distintos elementos UI
	/// dentro. Para cada layout se deberá sobreescribir esta clase.
	class OSKAPI_CALL IContainer : public IElement {

	protected:

		explicit IContainer(const Vector2f& size) : IElement(size) {  }

	public:

		void SetSize(Vector2f newSize) override;
		void _SetPosition(const Vector2f& newPosition) override;

	public:

		/// @brief Ajusta el tamaño del contenedor, para que sea el justo y
		/// necesario para que quepan todos los elementos hijos.
		void AdjustSizeToChildren();

		/// @brief Añade un elemento al contenedor. 
		/// Modificará su posición dependiendo del layout implementado.
		/// 
		/// @param key Identificador del elemento. Debe ser único dentro de este contenedor.
		/// @param child Referencia compartida al elemento.
		/// 
		/// @pre El elemento @p child no debe estar ya añadido a ningún contenedor.
		/// @pre Este contenedor no debe tener ningún elemento con el identificador @p key.
		void AddChild(const std::string& key, UniquePtr<IElement>&& child);

		/// @brief Reestructura todos los hijos del contenedor.
		void Rebuild();


		/// @brief Renderiza todos los elementos contenidos.
		void Render(GRAPHICS::ISdfRenderer2D* renderer) const override;

		bool UpdateByCursor(Vector2f cursorPosition, bool isPressed) override;
		void UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current) override;

		/// @param name Nombre del elemento hijo.
		/// @return Puntero al elemento indicado.
		/// @pre Debe haberse introducido un elemento con el nombre dado.
		IElement* GetChild(const std::string_view name);
		const IElement* GetChild(const std::string_view name) const;


		/// @brief Elimina el elemento indicado.
		/// @param childName Nombre del elemento.
		/// 
		/// @pre Debe existir un elemento con nombre @p childName.
		/// @throws InvalidArgumentException si se incumple
		/// la precondición.
		void DeleteChild(std::string_view childName);

		/// @param childName Nombre del elemento hijo.
		/// @return True si existe, false en caso contrario.
		bool HasChild(std::string_view childName) const;

	protected:

		/// @brief Resetea el layout, de tal manera que los siguientes
		/// elementos colocados se coloquen en el inicio del layout.
		virtual void ResetLayout() = 0;

		/// @brief Coloca un hijo en la posición deseada, según el layout.
		/// @param child Hijo a cambiar de posición.
		virtual void EmplaceChild(IElement* child) = 0;

	protected:

		DynamicArray<UniquePtr<IElement>> m_children;
		std::unordered_map<std::string, IElement*, StringHasher, std::equal_to<>> m_childrenTable{};

	};

}
