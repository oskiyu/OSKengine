#pragma once

namespace OSK {
	
	/*
	Comportamiento del array cuando no hay espacio:
	-EXPONENTIAL: se amplia de manera exponencial (Capacity * Factor).
	-EXPONENTIAL: se amplia de manera lineal (Capacity + Factor).
	*/
	enum class GrowthFactorType {
		EXPONENTIAL,
		LINEAL
	};

}
