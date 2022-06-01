#pragma once

#include "DynamicArray.hpp"

#include <string>

namespace OSK::GRAPHICS {

	/// <summary>
	/// Informaci�n de los atributos de un v�rtice.
	/// El graphics pipeline lo usar� para configurar el tipo
	/// de v�rtice deseado.
	/// </summary>
	class VertexInfo {

	public:

		/// <summary>
		/// Informaci�n de un atributo.
		/// </summary>
		struct Entry {

			/// <summary>
			/// Tipo de dato base del atributo.
			/// </summary>
			enum class Type {
				INT,
				FLOAT
			};

			Entry(TSize size, Type type, const std::string& name) 
				: size(size), type(type), name(name) {

			}

			/// <summary>
			/// Tama�o (en n�mero de bytes).
			/// </summary>
			TSize size = 0;
			Type type = Type::FLOAT;
			
			void SetName(const std::string& name) {
				this->name = name;
			}

			const std::string& GetName() const {
				return name;
			}

		private:

			std::string name;

		};

		/// <summary>
		/// Una entrada por atributo.
		/// </summary>
		DynamicArray<Entry> entries;

	};

}
