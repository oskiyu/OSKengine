#pragma once

#include "ECS.h"

#include "RenderizableScene.h"
#include "RenderStage.h"

namespace OSK {

	/// <summary>
	/// Sistema ECS para el renderizado de modelos 3D.
	/// </summary>
	class RenderSystem3D : public ECS::System {

	public:
				
		/// <summary>
		/// Debe iniciarse desués de OnCreate.
		/// </summary>
		void Init();

		/// <summary>
		/// Renderiza los modelos 3D.
		/// </summary>
		/// <param name="cmdBuffer">Buffer de comandos de Vulkan.</param>
		/// <param name="i">Iteración.</param>
		void OnDraw(VkCommandBuffer cmdBuffer, uint32_t i) override;

		/// <summary>
		/// Función OnTick.
		/// Actualiza los modelos 3D.
		/// </summary>
		/// <param name="deltaTime"></param>
		void OnTick(deltaTime_t deltaTime) override;

		/// <summary>
		/// Obtiene el Signature (componentes necesarios de un objeto) del sistema.
		/// </summary>
		/// <returns>Signature.</returns>
		Signature GetSystemSignature() override;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* Renderer = nullptr;

		/// <summary>
		/// Escena 3D renderizada.
		/// </summary>
		UniquePtr<RenderizableScene> RScene;

		/// <summary>
		/// Render stage que contiene los spritebatches.
		/// </summary>
		RenderStage Stage;

	};

}
