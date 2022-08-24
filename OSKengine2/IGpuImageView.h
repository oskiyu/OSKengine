#pragma once

#include "OSKmacros.h"
#include "GpuImageUsage.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Representa el uso que se le va a dar a la imagen
	/// al acceder a traves de un view dado.
	/// </summary>
	enum class ViewUsage {

		/// <summary>
		/// Read-only en el fragment shader.
		/// </summary>
		SAMPLED,

		/// <summary>
		/// Se renderizar� una imagen al canal de color
		/// usando un graphics pipeline.
		/// </summary>
		COLOR_TARGET,

		/// <summary>Se renderizar� una imagen al canal de profundidad
		/// usando un graphics pipeline.
		/// </summary>
		DEPTH_STENCIL_TARGET,

		/// <summary>
		/// Uso como read-write en shaders de computaci�n y
		/// ray-tracing.
		/// </summary>
		STORAGE

	};


	/// <summary>
	/// Un image view describe c�mo y a q�e parte de una imagen
	/// se va a acceder al usarla.
	/// </summary>
	class OSKAPI_CALL IGpuImageView {

	public:

		IGpuImageView(SampledChannel channel, SampledArrayType arrayType, TSize baseArrayLevel, TSize layerCount, ViewUsage usage);

		OSK_DEFINE_AS(IGpuImageView);

		/// <summary>
		/// Devuelve el canal al que se va a acceder a trav�s
		/// de este view.
		/// </summary>
		SampledChannel GetChannel() const;

		/// <summary>
		/// Permite saber si este view consiste en una �nica capa
		/// o referencia varias capas en un array.
		/// </summary>
		SampledArrayType GetArrayType() const;

		/// <summary>
		/// Devuelve el �ndice de la primera capa del array
		/// (o de la �nica capa si GetArrayType() es
		/// SampledArrayType::SINGLE_LAYER.
		/// </summary>
		TSize GetBaseArrayLevel() const;

		/// <summary>
		/// N�mero de capas del array.
		/// </summary>
		/// 
		/// @note Si GetArrayType() es
		/// SampledArrayType::SINGLE_LAYER,
		/// devolver� 1.
		TSize GetLayerCount() const;

		/// <summary>
		/// Contexto en el que se va a usar la imagen.
		/// </summary>
		ViewUsage GetViewUsage() const;

		bool operator==(const IGpuImageView& other);

	private:

		SampledChannel channel = SampledChannel::COLOR;
		SampledArrayType arrayType = SampledArrayType::ARRAY;

		TSize baseArrayLevel = 0;
		TSize layerCount = 0;

		ViewUsage usage = ViewUsage::SAMPLED;

	};

}

template<> struct std::hash<OSK::GRAPHICS::IGpuImageView> {
	std::size_t operator()(const OSK::GRAPHICS::IGpuImageView& view) const {
		return std::hash<std::size_t>()((std::size_t)(view.GetArrayType()));
		// @todo improve
	}
};
