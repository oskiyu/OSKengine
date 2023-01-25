#pragma once

#include "OSKmacros.h"
#include "DataType.h"

namespace OSK::PERSISTENCE {

	/// @brief Clase base para cualquier dato almacenado en un nodo de persistencia.
	class OSKAPI_CALL IDataElement {

	public:

		OSK_DEFINE_AS(IDataElement);
		OSK_DISABLE_COPY(IDataElement);

		/// @brief Tipo de dato. Puede ser un wrapper de un atributo o un 
		/// nodo hijo.
		/// @return Tipo de dato almacenado.
		inline DataType GetDataType() const {
			return type;
		}

	protected:

		/// @brief Crea el elemento.
		/// Se debe especificar el tipo de dato de la clase ahijada.
		/// El tipo de dato no puede ser UNKNOWN.
		/// @param type Tipo de dato.
		inline IDataElement(DataType type) : type(type) { }

	private:

		/// @brief Tipo de elemento de dato.
		/// No debe ser UNKNWONW.
		DataType type = DataType::UNKNOWN;

	};

}
