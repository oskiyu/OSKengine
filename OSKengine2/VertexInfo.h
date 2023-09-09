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

		VertexInfo() = default;
		OSK_DEFAULT_COPY_OPERATOR(VertexInfo);

		/// <summary>
		/// Información de un atributo.
		/// </summary>
		struct Entry {

			Entry() = default;
			OSK_DEFAULT_COPY_OPERATOR(Entry);

			/// <summary>
			/// Tipo de dato base del atributo.
			/// </summary>
			enum class Type {
				INT,
				FLOAT
			};

			Entry(USize32 size, Type type, const std::string& name) 
				: size(size), type(type), name(name) {

			}

			/// <summary>
			/// Tamaño (en número de bytes).
			/// </summary>
			USize32 size = 0;
			Type type = Type::FLOAT;
			
			void SetName(const std::string& nname) {
				this->name = nname;
			}

			const std::string& GetName() const {
				return name;
			}

		private:

			std::string name;

		};

		/// Una entrada por atributo.
		DynamicArray<Entry> entries;

		USize32 GetSize() const;

	};

}
