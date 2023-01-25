// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "Data.h"

namespace OSK::PERSISTENCE {


	/// @brief Interfaz para una clase que puede ser serializada.
	class OSKAPI_CALL ISerializable {

	public:

		/// @brief Crea un nodo con los datos de la clase.
		/// @return Nodo con los datos de la clase.
		virtual DataNode Serialize() const = 0;

		/// @brief Construye la clase a partir de un nodo.
		/// @param node Nodo con los datos deserializados de la clase.
		virtual void Deserialize(const DataNode& node) = 0;
	};

}
