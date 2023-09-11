#pragma once

#include "RenderTarget.h"
#include <array>

namespace OSK::GRAPHICS {

	class ICommandList;


	/// @brief Clase que contiene los render targets
	/// necesarios para renderizar un GBuffer. Un GBuffer contiene
	/// im�genes con la informaci�n de posici�n, color, normales
	/// y movimiento de la geometr�a que aparece en la pantalla, de tal
	/// manera que permite un renderizado en diferido.
	class OSKAPI_CALL GBuffer {

	public:

		/// @brief Identifica cada una de las im�genes
		/// del GBuffer.
		enum class Target : UIndex32 {
			/// @brief Color = rgb uint.
			/// Stencil = a uint.
			COLOR = 0,
			/// @brief Normal = xyz.
			/// Alpha = a.
			NORMAL = 1,
			/// @brief Metallic = x, roughness = y.
			METALLIC_ROUGHNESS = 2,
			/// @brief Movimiento = xy
			MOTION = 3,

			DEPTH = std::numeric_limits<UIndex32>::max()
		};

		/// @brief Contiene todos los tipos de im�genes, excluyendo la de profundidad.
		const static std::array<Target, 4> ColorTargetTypes;


		/// @brief Crea el GBuffer con la informaci�n dada.
		/// @param resolution Resoluci�n, en p�xeles.
		/// @param sampler Sampler usado por todas las im�genes de color.
		/// @param usage Uso de las im�genes de color. Tendr�, al menos, los definidos en RenderTarget.
		void Create(
			const Vector2ui& resolution, 
			GpuImageSamplerDesc sampler = GpuImageSamplerDesc::CreateDefault(), 
			GpuImageUsage usage = GpuImageUsage::COLOR | GpuImageUsage::SAMPLED);

		/// @brief Cambia la resoluci�n del GBuffer.
		/// @see RenderTarget
		/// @param resolution Nueva resoluci�n, en p�xeles.
		void Resize(const Vector2ui& resolution);


		/// @brief Devuelve la imagen indicada por los par�metros.
		/// @param frameIndex �ndice del recurso.
		/// @param targetType Identificador de la imagen deseada.
		/// 
		/// @pre frameIndex < NUM_RESOURCES_IN_FLIGHT
		GpuImage* GetImage(UIndex32 frameIndex, Target targetType);

		/// @brief Devuelve la imagen indicada por los par�metros.
		/// @param frameIndex �ndice del recurso.
		/// @param targetType Identificador de la imagen deseada.
		/// 
		/// @pre frameIndex < NUM_RESOURCES_IN_FLIGHT
		const GpuImage* GetImage(UIndex32 frameIndex, Target targetType) const;


		/// @brief Inicia un renderpass de rasterizaci�n sobre el GBuffer.
		/// @see ICommandList::BeginRenderpass.
		/// @param cmdList Lista de comandos abierta.
		/// @param color Color con el que se limpiar�n todas las im�genes de color.
		void BeginRenderpass(ICommandList* cmdList, Color color);

	private:

		RenderTarget renderTarget;

	};

}
