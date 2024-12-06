#pragma once

#include "UiElement.h"

#include "UiTextView.h"
#include "UiImageView.h"

#include "DynamicArray.hpp"
#include "HashMap.hpp"

#include <functional>

namespace OSK::UI {

	/// @brief Elemento de UI que permite al usuario seleccionar una opci�n entre
	/// varias mediante un men� desplegable.
	class OSKAPI_CALL Dropdown : public IElement {

	public:

		/// @brief Representa una de las opciones seleccionables.
		class Element : public IElement {

		public:

			/// @brief Estado en el que puede estar el elemento.
			enum class State {

				/// @brief Estado por defecto.
				DEFAULT,

				/// @brief El rat�n est� por encima.
				HOVERED

			};

		public:

			/// @brief Crea el elemento.
			/// @param size Tama�o ocupado por el elemento.
			/// @param text Texto de la opci�n.
			explicit Element(const Vector2f& size, const std::string& text);

			void Render(GRAPHICS::ISdfRenderer2D* renderer) const override;
			void _SetPosition(const Vector2f& newPosition) override;


			/// @brief Establece la fuente del texto.
			/// @param fuente Fuente del texto.
			/// Si es null, no renderizar� texto.
			/// @param fontSize Tama�o del texto.
			void SetFont(ASSETS::AssetRef<ASSETS::Font> fuente, USize32 fontSize);


			/// @brief Establece la imagen de fondo.
			/// Si es null, no tendr� fondo.
			/// @param view Imagen de fondo. Si es null, no tendr� fondo.
			/// @param defaultColor Color que tendr� en su estado por defecto.
			/// @param hoveredColor Color que tendra en el estado State::HOVERED.
			void SetBackground(
				const GRAPHICS::IGpuImageView* view,
				const Color& defaultColor,
				const Color& hoveredColor);


			/// @return Texto de la opci�n.
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

		/// @brief Funci�n que se ejecuta cuando cambia el elemento seleccionado.
		using CallbackFnc = std::function<void(std::string_view selectedText)>;

	public:

		/// @brief Crea el dropdown.
		/// @param size Tama�o del dropdown (y de cada una de las ociones).
		explicit Dropdown(const Vector2f& size);

		void Render(GRAPHICS::ISdfRenderer2D* renderer) const override;
		bool UpdateByCursor(Vector2f cursor, bool isPressed) override;

		/// @brief A�ade una nueva opci�n al dropdown.
		/// @param text Texto de la opci�n. �nico en el dropdown.
		/// Si el texto coincide con otra opci�n existente en el dropdown,
		/// se eliminar� dicha opci�n y se a�adir� esta.
		void AddElement(const std::string& text);

		/// @return True si contiene un elemento con el texto dado.
		bool ContainsElement(std::string_view text) const;

		/// @brief Elimina una de las opciones.
		/// @param text Texto de la opci�n.
		/// Si no hay ninguna opci�n con ese texto,
		/// no ocurrir� nada.
		void RemoveElement(std::string_view text);


		/// @brief Selecciona una de las opciones, ejecutando
		/// el callback.
		/// @param selectedText Texto de la opci�n elegida.
		/// Si no hay ninguna opci�n con el texto indicado,
		/// no ocurrir� nada.
		void SelectOption(std::string_view selectedText);


		/// @brief Establece el callback que se ejecutar� cada vez que
		/// se cambie de opci�n.
		/// @param callback Callback que se ejecutar�.
		void SetSelectionCallback(const CallbackFnc& callback);


		/// @brief Establece la fuente que usar�n las opciones.
		/// Se aplicar� a las opciones ya existentes y a las opciones
		/// que se a�adan despu�s.
		/// @param fuente Fuente. Si es null, no se renderizar� el texto.
		/// @param fontSize Tama�o de la fuente.
		void SetFont(ASSETS::AssetRef<ASSETS::Font> fuente, USize32 fontSize);


		/// @brief Establece la imagen de fondo que usar�n las opciones.
		/// Se aplicar� a las opciones ya existentes y a las opciones
		/// que se a�adan despu�s.
		/// @param view Imagen de fondo. Si es null, no tendr� imagen de fondo.
		void SetBackground(
			const GRAPHICS::IGpuImageView* view,
			const OSK::Color& defaultColor,
			const OSK::Color& hoverColor);

	private:

		/// @brief Estados en los que puede estar el dropdown.
		enum class State {

			/// @brief Estado por defecto, s�lamente muestra la
			/// opci�n seleccionada.
			DEFAULT,

			/// @brief Muestra todas las opciones en un desplegable.
			EXPANDED

		};

	private:

		/// @brief Estado actual.
		State m_state = State::DEFAULT;

		/// @brief Mapa [texto de la opci�n] -> [�ndice que ocupa en la lista].
		std::unordered_map<std::string, UIndex64, StringHasher, std::equal_to<>>
			m_elementsMap;

		/// @brief Lista con todos los elementos.
		DynamicArray<Element> m_elements;

		/// @brief �ndice con el elemento seleccionado.
		UIndex64 m_selectedElement = 0;


		/// @brief Callback a ejecutar cada vez que se cambie de opci�n seleccionada.
		CallbackFnc m_callback = [](std::string_view) { (void)0; };

		/// @brief True si el fotograma anterior estaba siendo pulsado.
		bool m_wasPreviousFramePressed = false;


		/// @brief Fuente de todas las opciones.
		/// Para aplic�rsela a las siguientes opciones que se a�adan.
		ASSETS::AssetRef<ASSETS::Font> m_fuente;

		/// @brief Tama�o de fuente de todas las opciones.
		/// Para aplic�rselo a las siguientes opciones que se a�adan.
		USize32 m_fontSize = 22;


		/// @brief Imagen de fondo de todas las opciones.
		/// Para aplic�rsela a las siguientes opciones que se a�adan.
		const GRAPHICS::IGpuImageView* m_elementsBackground = nullptr;

		/// @brief Color del fondo por defecto.
		Color m_defaultColor = Color::White;

		/// @brief Color del fondo cuando el rat�n est� por encima.
		Color m_hoverColor = Color::White;

	};

}
