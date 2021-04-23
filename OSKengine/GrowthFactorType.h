#pragma once

namespace OSK {
	
	/// <summary>
	/// Comportamiento del array cuando no hay espacio:
	/// -EXPONENTIAL: se amplia de manera exponencial(Capacity * Factor).
	///	-LINEAL: se amplia de manera lineal(Capacity + Factor).
	/// </summary>
	enum class GrowthFactorType {

		/// <summary>
		/// Se amplia de manera exponencial(Capacity * Factor).
		/// </summary>
		EXPONENTIAL,

		/// <summary>
		/// Se amplia de manera lineal(Capacity + Factor).
		/// </summary>
		LINEAL

	};

}
