export module OSKengine.Input.MouseState;

export import OSKengine.Types.Vector2;

export namespace OSK {

	/// <summary>
	/// Botones del ratón.
	/// </summary>
	enum class MouseButton {

		/// <summary>
		/// Botón extra #1.
		/// </summary>
		BUTTON_1,

		/// <summary>
		/// Botón extra #2.
		/// </summary>
		BUTTON_2,

		/// <summary>
		/// Botón extra #3.
		/// </summary>
		BUTTON_3,

		/// <summary>
		/// Botón extra #4.
		/// </summary>
		BUTTON_4,

		/// <summary>
		/// Botón extra #5.
		/// </summary>
		BUTTON_5,

		/// <summary>
		/// Botón extra #6.
		/// </summary>
		BUTTON_6,

		/// <summary>
		/// Botón extra #7.
		/// </summary>
		BUTTON_7,

		/// <summary>
		/// Botón izquierdo.
		/// </summary>
		BUTTON_LEFT,

		/// <summary>
		/// Botón derecho.
		/// </summary>
		BUTTON_RIGHT,

		/// <summary>
		/// Botón de la rueda.
		/// </summary>
		BUTTON_MIDDLE,

		__END

	};

	/// <summary>
	/// Estados en el que puede estar un botón.
	/// </summary>
	enum class ButtonState {

		/// <summary>
		/// No está siendo pulsado.
		/// </summary>
		RELEASED,

		/// <summary>
		/// Está siendo pulsado.
		/// </summary>
		PRESSED

	};

	/// <summary>
	/// Número de teclas disponibles en el teclado.
	/// </summary>
	constexpr int MouseNumberOfButtons = (int)MouseButton::__END;

	/// <summary>
	/// Struct que almacena el estado del ratón en un determinado momento.
	/// Contiene la posición, el scroll y el estado de los botones.
	/// </summary>
	struct MouseState {

	public:

		/// <summary>
		/// Obtiene el estado de un botón.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		ButtonState GetButtonState(MouseButton button) const {
			return buttonStates[(int)button];
		}

		/// <summary>
		/// Obtiene si un botón está siendo pulsado.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		bool IsButtonDown(MouseButton button) const {
			return GetButtonState(button) == ButtonState::PRESSED;
		}

		/// <summary>
		/// Obtiene si un botón no está siendo pulsado.
		/// </summary>
		/// <param name="button">Botón del ratón.</param>
		/// <returns>Estado.</returns>
		bool IsButtonUp(MouseButton button) const {
			return !IsButtonDown(button);
		}

		/// <summary>
		/// Posición en píxeles respecto a la esquina superior izquierda de la ventana.
		/// </summary>
		Vector2i GetPosition() const {
			return position;
		}

		/// <summary>
		/// Posición en porcentaje respecto a la esquina superior izquierda de la ventana.
		/// Entre 0.0 y 1.0.
		/// 0.0 = borde izquierdo.
		/// 1.0 = borde derecho.
		/// </summary>
		Vector2 GetRelativePosition() const {
			return relativePosition;
		}

		/// <summary>
		/// Rueda del ratón vertical (la normal).
		/// </summary>
		int GetScroll() const {
			return scroll.Y;
		}

		/// <summary>
		/// Rueda del ratón horizontal.
		/// </summary>
		int GetHorizontalScroll() const {
			return scroll.X;
		}

		void _SetScrollX(int x) {
			scroll.X = x;
		}
		void _SetScrollY(int y) {
			scroll.Y = y;
		}
		void _SetPosition(const Vector2i& pos) {
			position = pos;
		}
		void _SetRelativePosition(const Vector2& pos) {
			relativePosition = pos;
		}
		void _SetButtonState(MouseButton button, ButtonState state) {
			buttonStates[(int)button] = state;
		}

	private:

		Vector2i position;
		Vector2 relativePosition;
		Vector2i scroll;

		ButtonState buttonStates[MouseNumberOfButtons];

	};

}
