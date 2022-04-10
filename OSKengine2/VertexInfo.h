#pragma once

#include "DynamicArray.hpp"

#include <string>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Información de los atributos de un vértice.
	/// El graphics pipeline lo usará para configurar el tipo
	/// de vértice deseado.
	/// </summary>
	class VertexInfo {

	public:

		/// <summary>
		/// Información de un atributo.
		/// </summary>
		struct Entry {

			/// <summary>
			/// Tipo de dato base del atributo.
			/// </summary>
			enum class Type {
				INT,
				FLOAT
			};

			/// <summary>
			/// Tamaño (en número de bytes).
			/// </summary>
			TSize size = 0;
			Type type = Type::FLOAT;
			std::string name;
		};

		/// <summary>
		/// Una entrada por atributo.
		/// </summary>
		DynamicArray<Entry> entries;

	};

}
