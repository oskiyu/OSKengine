#pragma once

#include <string>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Modo de renderizado de los polígonos: llenos o lineas.
	/// </summary>
	enum class PolygonMode {

		/// <summary>
		/// Se renderizan los polígonos enteros.
		/// </summary>
		FILL,

		/// <summary>
		/// Se renderizan los bordes.
		/// </summary>
		LINE

	};


	/// <summary>
	/// Modo de cull de los polígonos.
	/// Algunos polígonos pueden descartarse si dan la espalda a la cámara.
	/// </summary>
	enum class PolygonCullMode {

		/// <summary>
		/// Se descartan los polígonos que miran hacia la cámara.
		/// </summary>
		FRONT,

		/// <summary>
		/// Se descartan los polígonos que no miran hacia la cámara.
		/// </summary>
		BACK,

		/// <summary>
		/// No se descarta ningún polígono.
		/// </summary>
		NONE

	};

	/// <summary>
	/// Qué cara del polígono mira hacia delante.
	/// </summary>
	enum class PolygonFrontFaceType {

		/// <summary>
		/// Mira hacia delante si los vértices están en orden de las agujas del reloj.
		/// </summary>
		CLOCKWISE,

		/// <summary>
		/// /// Mira hacia delante si los vértices están en orden contrario de las agujas del reloj.
		/// </summary>
		COUNTERCLOCKWISE

	};

	/// <summary>
	/// Establece la configuración de testeo de profundidad.
	/// Activarlo permite que los objetos en la imagen final sean los que más
	/// cerca están de la cámara.
	/// </summary>
	enum class DepthTestingType {

		/// <summary>
		/// Ninguna operación.
		/// </summary>
		NONE,

		/// <summary>
		/// Lee la información para comprobar que el objeto debe ser renderizado,
		/// pero no actualiza la información.
		/// </summary>
		READ,

		/// <summary>
		/// Comportamiento normal.
		/// </summary>
		READ_WRITE

	};

	/// <summary>
	/// Contiene la información necesaria para configurar un nuevo graphics pipeline.
	/// </summary>
	struct PipelineCreateInfo {

		/// <summary>
		/// Dirección en la que se encuentra el shader de vértices.
		/// 
		/// @note Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		/// </summary>
		std::string vertexPath;

		/// <summary>
		/// Dirección en la que se encuentra el shader de fragmentos o píxeles.
		/// 
		/// @note Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		/// </summary>
		std::string fragmentPath;

		/// <summary>
		/// Tipo de polígono (línea, triángulo...).
		/// </summary>
		PolygonMode polygonMode;

		/// <summary>
		/// Establece si no se renderizarán los polígonos que estén de espaldas
		/// a la cámara.
		/// </summary>
		PolygonCullMode cullMode = PolygonCullMode::FRONT;

		/// <summary>
		/// Establece cómo saber qué cara es la delantera del triángulo.
		/// Para poder usar correctamente CullMode.
		/// 
		/// Debe elegirse si la cara de frente tiene los vértices definidos en sentido
		/// horario o antihorario.
		/// </summary>
		PolygonFrontFaceType frontFaceType;

		/// <summary>
		/// Establece la configuración de testeo de profundidad.
		/// Activarlo permite que los objetos en la imagen final sean los que más
		/// cerca están de la cámara.
		/// </summary>
		DepthTestingType depthTestingType = DepthTestingType::READ_WRITE;

	};

}
