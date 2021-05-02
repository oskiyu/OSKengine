#pragma once

#include "ECS.h"

namespace OSK {

	/// <summary>
	/// Sistema que maneja los componentes OnTick.
	/// </summary>
	class OnTickSystem : public ECS::System {

	public:

		/// <summary>
		/// Ejecuta la función OnTick de los componentes.
		/// </summary>
		/// <param name="deltaTime">Delta.</param>
		void OnTick(deltaTime_t deltaTime) override;

		/// <summary>
		/// Obtiene el Signature (componentes necesarios de un objeto) del sistema.
		/// </summary>
		/// <returns>Signature.</returns>
		Signature GetSystemSignature() override;

	};

}