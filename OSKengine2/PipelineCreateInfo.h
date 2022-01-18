#pragma once

#include <string>

namespace OSK {

	/// <summary>
	/// Modo de renderizado de los pol�gonos: llenos o lineas.
	/// </summary>
	enum class PolygonMode {

		/// <summary>
		/// Se renderizan los pol�gonos enteros.
		/// </summary>
		FILL,

		/// <summary>
		/// Se renderizan los bordes.
		/// </summary>
		LINE

	};


	/// <summary>
	/// Modo de cull de los pol�gonos.
	/// Algunos pol�gonos pueden descartarse si dan la espalda a la c�mara.
	/// </summary>
	enum class PolygonCullMode {

		/// <summary>
		/// Se descartan los pol�gonos que miran hacia la c�mara.
		/// </summary>
		FRONT,

		/// <summary>
		/// Se descartan los pol�gonos que no miran hacia la c�mara.
		/// </summary>
		BACK,

		/// <summary>
		/// No se descarta ning�n pol�gono.
		/// </summary>
		NONE

	};

	/// <summary>
	/// Qu� cara del pol�gono mira hacia delante.
	/// </summary>
	enum class PolygonFrontFaceType {

		/// <summary>
		/// Mira hacia delante si los v�rtices est�n en orden de las agujas del reloj.
		/// </summary>
		CLOCKWISE,

		/// <summary>
		/// /// Mira hacia delante si los v�rtices est�n en orden contrario de las agujas del reloj.
		/// </summary>
		COUNTERCLOCKWISE

	};


	/// <summary>
	/// Contiene la informaci�n necesaria para configurar un nuevo graphics pipeline.
	/// </summary>
	struct PipelineCreateInfo {

		/// <summary>
		/// Direcci�n en la que se encuentra el shader de v�rtices.
		/// 
		/// Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		/// </summary>
		std::string vertexPath;

		/// <summary>
		/// Direcci�n en la que se encuentra el shader de fragmentos o p�xeles.
		/// 
		/// Debe ser un archivo ya compilado, en caso de que se use el renderizador
		/// DirectX12 o Vulkan.
		/// </summary>
		std::string fragmentPath;

		/// <summary>
		/// Tipo de pol�gono (l�nea, tri�ngulo...).
		/// </summary>
		PolygonMode polygonMode;

		/// <summary>
		/// Establece si no se renderizar�n los pol�gonos que est�n de espaldas
		/// a la c�mara.
		/// </summary>
		PolygonCullMode cullMode;

		/// <summary>
		/// Establece c�mo saber qu� cara es la delantera del tri�ngulo.
		/// Para poder usar correctamente CullMode.
		/// 
		/// Debe elegirse si la cara de frente tiene los v�rtices definidos en sentido
		/// horario o antihorario.
		/// </summary>
		PolygonFrontFaceType frontFaceType;

	};

}