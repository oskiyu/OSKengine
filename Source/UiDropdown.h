#pragma once

#include "UiElement.h"

#include "UiTextView.h"
#include "UiImageView.h"

#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include <functional>

namespace OSK::UI {

	/// @brief Elemento de UI que permite al usuario seleccionar una opción entre
	/// varias mediante un menú desplegable.
	class OSKAPI_CALL Dropdown : public IElement {

	public:

		/// @brief Representa una de las opciones seleccionables.
		class Element : public IElement {

		public:

			/// @brief Estado en el que puede estar el elemento.
			enum class State {

				/// @brief Estado por defecto.
				DEFAULT,

				/// @brief El ratón está por encima.
				HOVERED

			};

		public:

			/// @brief Crea el elemento.
			/// @param size Tamaño ocupado por el elemento.
			/// @param text Texto de la opción.
			explicit Element(const Vector2f& size, const std::string& text);

			void Render(GRAPHICS::ISdfRenderer2D* renderer) const override;
			void _SetPosition(const Vector2f& newPosition) override;


			/// @brief Establece la fuente del texto.
			/// @param fuente Fuente del texto.
			/// Si es null, no renderizará texto.
			/// @param fontSize Tamaño del texto.
			void SetFont(ASSETS::AssetRef<ASSETS::Font> fuente, USize32 fontSize);


			/// @brief Establece la imagen de fondo.
			/// Si es null, no tendrá fondo.
			/// @param view Imagen de fondo. Si es null, no tendrá fondo.
			/// @param defaultColor Color que tendrá en su estado por defecto.
			/// @param hoveredColor Color que tendra en el estado State::HOVERED.
			void SetBackground(
				const GRAPHICS::IGpuImageView* view,
				const Color& defaultColor,
				const Color& hoveredColor);


			/// @return Texto de la opción.
			std::string_view GetText() const;

			/// @brief Actualiza el estado del elemento.
			/// @param state Nuevo estado.
			void SetState(State state);

		private:

			Vector2f GetTextOffset() const;

			State m_state = State::DEFAULT;

			TextView m_textView;
			ImageView m_defaultImageView;
			ImageView m_hoveredImageView;

		};

	public:

		/// @brief Función que se ejecuta cuando cambia el elemento seleccionado.
		using CallbackFnc = std::function<void(std::string_view selectedText)>;

	public:

		/// @brief Crea el dropdown.
		/// @param size Tamaño del dropdown (y de cada una de las ociones).
		explicit Dropdown(const Vector2f& size);

		void Render(GRAPHICS::ISdfRenderer2D* renderer) const override;
		bool UpdateByCursor(Vector2f cursor, bool isPressed) override;

		/// @brief Añade una nueva opción al dropdown.
		/// @param text Texto de la opción. Único en el dropdown.
		/// Si el texto coincide con otra opción existente en el dropdown,
		/// se eliminará dicha opción y se añadirá esta.
		void AddElement(const std::string& text);

		/// @return True si contiene un elemento con el texto dado.
		bool ContainsElement(std::string_view text) const;

		/// @brief Elimina una de las opciones.
		/// @param text Texto de la opción.
		/// Si no hay ninguna opción con ese texto,
		/// no ocurrirá nada.
		void RemoveElement(std::string_view text);


		/// @brief Selecciona una de las opciones, ejecutando
		/// el callback.
		/// @param selectedText Texto de la opción elegida.
		/// Si no hay ninguna opción con el texto indicado,
		/// no ocurrirá nada.
		void SelectOption(std::string_view selectedText);


		/// @brief Establece el callback que se ejecutará cada vez que
		/// se cambie de opción.
		/// @param callback Callback que se ejecutará.
		void SetSelectionCallback(const CallbackFnc& callback);


		/// @brief Establece la fuente que usarán las opciones.
		/// Se aplicará a las opciones ya existentes y a las opciones
		/// que se añadan después.
		/// @param fuente Fuente. Si es null, no se renderizará el texto.
		/// @param fontSize Tamaño de la fuente.
		void SetFont(ASSETS::AssetRef<ASSETS::Font> fuente, USize32 fontSize);


		/// @brief Establece la imagen de fondo que usarán las opciones.
		/// Se aplicará a las opciones ya existentes y a las opciones
		/// que se añadan después.
		/// @param view Imagen de fondo. Si es null, no tendrá imagen de fondo.
		void SetBackground(
			const GRAPHICS::IGpuImageView* view,
			const OSK::Color& defaultColor,
			const OSK::Color& hoverColor);

	private:

		/// @brief Estados en los que puede estar el dropdown.
		enum class State {

			/// @brief Estado por defecto, sólamente muestra la
			/// opción seleccionada.
			DEFAULT,

			/// @brief Muestra todas las opciones en un desplegable.
			EXPANDED

		};

	private:

		/// @brief Estado actual.
		State m_state = State::DEFAULT;

		/// @brief Mapa [texto de la opción] -> [índice que ocupa en la lista].
		std::unordered_map<std::string, UIndex64, StringHasher, std::equal_to<>>
			m_elementsMap;

		/// @brief Lista con todos los elementos.
		DynamicArray<Element> m_elements;

		/// @brief Índice con el elemento seleccionado.
		UIndex64 m_selectedElement = 0;


		/// @brief Callback a ejecutar cada vez que se cambie de opción seleccionada.
		CallbackFnc m_callback = [](std::string_view) { (void)0; };

		/// @brief True si el fotograma anterior estaba siendo pulsado.
		bool m_wasPreviousFramePressed = false;


		/// @brief Fuente de todas las opciones.
		/// Para aplicársela a las siguientes opciones que se añadan.
		ASSETS::AssetRef<ASSETS::Font> m_fuente;

		/// @brief Tamaño de fuente de todas las opciones.
		/// Para aplicárselo a las siguientes opciones que se añadan.
		USize32 m_fontSize = 22;


		/// @brief Imagen de fondo de todas las opciones.
		/// Para aplicársela a las siguientes opciones que se añadan.
		const GRAPHICS::IGpuImageView* m_elementsBackground = nullptr;

		/// @brief Color del fondo por defecto.
		Color m_defaultColor = Color::White;

		/// @brief Color del fondo cuando el ratón está por encima.
		Color m_hoverColor = Color::White;

	};

}
