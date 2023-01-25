// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#pragma once

#include "Data.h"
#include "ISerializable.h"

namespace OSK::PERSISTENCE {

	/// @brief Interfaz para una clase que puede serializar / deserializar objetos.
	class OSKAPI_CALL ISerializer {

	public:

		/// @brief Obtiene los datos de un objeto serializado.
		/// @param path Path del archivo.
		/// @return Datos del archivo.
		/// 
		/// @pre El path debe apuntar a un archivo de serialización que se ha
		/// serializado por la misma clase que la va a deserializar.
		virtual DataNode Deserialize(const std::string& path) const = 0;

		/// @brief Serializa los datos de un objeto.
		/// @param root Objeto a serializar.
		/// @param path Archivo donde se guardarán los datos.
		virtual void Serialize(const ISerializable& root, const std::string& path) = 0;

	private:

		/// @brief Serializa un nodo en concreto.
		/// @param node Nodo con datos.
		virtual void SerializeNode(const DataNode& node) = 0;

	};

}
