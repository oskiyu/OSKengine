#pragma once

#include "IUiContainer.h"

namespace OSK::UI {

	/// @brief Layout que se divide en 5 espacios:
	/// - Norte (ariba, de izquierda a derecha).
	/// - Sur (abajo, de izquierda a derecha).
	/// - Este (izquierda, de arriba a abajo, respetando Norte y Sur).
	/// - Oeste (derecha, de arriba a abajo, respetando Norte y Sur).
	/// - Center (centro, con el espacio que sobre).
	/// 
	/// @warning No añadir mediante AddChild, usar AddChild_InPosition.
	class OSKAPI_CALL BorderLayout : public IContainer {

	public:

		/// @brief Posición de un elemento.
		enum class Position {
			/// @brief Ariba, de izquierda a derecha.
			NORTH,
			/// @brief Abajo, de izquierda a derecha.
			SOUTH,
			/// @brief Izquierda, de arriba a abajo, respetando Norte y Sur.
			EAST,
			/// @brief Derecha, de arriba a abajo, respetando Norte y Sur.
			WEST,
			/// @brief Centro, con el espacio que sobre.
			CENTER
		};

	public:

		explicit BorderLayout(const Vector2f& size);

		/// @brief Añade un elemento en la posición indicada.
		/// @param child Hijo.
		/// @param position Posición en la que se añadirá.
		/// @note Si ya existía un elemento en la posición indicada,
		/// se reemplazará.
		void AddChild_InPosition(
			UniquePtr<IElement>&& child,
			Position position);


		/// @param position Posición.
		/// @return True si hay un elemento en la
		/// posición @p position.
		bool HasChild(Position position) const;

		/// @param position Posición.
		/// @return Elemento en esa posició.
		/// 
		/// @pre Debe haber un elemento en esa posición (@see HasChild).
		/// @throws InvalidObjectStateException si se incumple la precondición.
		IElement* GetChild(Position position);
		const IElement* GetChild(Position position) const;

	private:

		void EmplaceChild(IElement* child) override;
		void ResetLayout() override;

		IElement* m_northElement = nullptr;
		IElement* m_southElement = nullptr;
		IElement* m_eastElement = nullptr;
		IElement* m_westElement = nullptr;
		IElement* m_centerElement = nullptr;

	};

}
