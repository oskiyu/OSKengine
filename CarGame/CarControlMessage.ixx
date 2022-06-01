export module CarControlMessage;

/// <summary>
/// Mensaje de control para un coche.
/// 
/// Debe especificarse su tipo y la intensidad de la acci�n.
/// </summary>
export struct CarControlMessage {

	/// <summary>
	/// Tipo de acci�n que representar� el mensaje.
	/// </summary>
	enum class Type {

		/// <summary>
		/// Aceleraci�n o frenada.
		/// </summary>
		ACCEL,

		/// <summary>
		/// Velocidad angular.
		/// </summary>
		TURN,

		/// <summary>
		/// Se detiene por completo.
		/// </summary>
		STOP

	} type;

	/// <summary>
	/// Intensidad del efecto.
	/// </summary>
	float amount = 0.0f;

	CarControlMessage(Type type, float amount) : type(type), amount(amount) { }

};
