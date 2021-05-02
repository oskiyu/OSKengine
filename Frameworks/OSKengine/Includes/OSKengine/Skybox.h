#pragma once

#include "Texture.h"
#include "Model.h"
#include "GraphicsPipeline.h"
#include "DescriptorSet.h"

#include "MaterialInstance.h"

namespace OSK {

	/// <summary>
	/// Representa un skybox:
	/// una caja 'gigante' que rodea una escena 3D.
	/// Se usa para renderizar el cielo.
	/// </summary>
	class OSKAPI_CALL Skybox {

		friend class RenderAPI;
		friend class ContentManager;

	public:

		//Enlaza el skybox - tanto el modelo como la textura.

		/// <summary>
		/// Enlaza el skybox - tanto el modelo como la textura.
		/// Para poder ser renerizado.
		/// </summary>
		/// <param name="commandBuffer">Command buffer.</param>
		/// <param name="pipeline">Pipeline usada.</param>
		/// <param name="i">Iteración.</param>
		void Bind(VkCommandBuffer commandBuffer, GraphicsPipeline* pipeline, uint32_t i) const;

		/// <summary>
		/// Renderiza el skybox.
		/// Debe haberse llamado antes a Bind().
		/// </summary>
		/// <param name="commandBuffer">Command buffer.</param>
		void Draw(VkCommandBuffer commandBuffer) const;

	private:

		/// <summary>
		/// Material del skybox.
		/// </summary>
		SharedPtr<MaterialInstance> Instance;

		/// <summary>
		/// Modelo del skybox (un cubo).
		/// STATIC.
		/// </summary>
		static ModelData* Model;

	};

}