export module CarControlMessage;

/// <summary>
/// Mensaje de control para un coche.
/// 
/// Debe especificarse su tipo y la intensidad de la acción.
/// </summary>
export struct CarControlMessage {

	/// <summary>
	/// Tipo de acción que representará el mensaje.
	/// </summary>
	enum class Type {

		/// <summary>
		/// Aceleración o frenada.
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
