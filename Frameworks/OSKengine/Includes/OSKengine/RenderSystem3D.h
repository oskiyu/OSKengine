#pragma once

#include "ECS.h"

#include "RenderizableScene.h"
#include "RenderStage.h"

class Game;

namespace OSK {

	/// <summary>
	/// Sistema ECS para el renderizado de modelos 3D.
	/// </summary>
	class OSKAPI_CALL RenderSystem3D : public ECS::System {

		friend class Game;

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

		RenderizableScene* GetRenderScene() {
			return renderScene.GetPointer();
		}

	private:

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

		/// <summary>
		/// Escena 3D renderizada.
		/// </summary>
		UniquePtr<RenderizableScene> renderScene;

		/// <summary>
		/// Render stage que contiene los spritebatches.
		/// </summary>
		RenderStage renderStage;

	};

}
