#pragma once

#include "OSKmacros.h"

#include <format>
#include <string>

namespace OSK {

	/// @brief Identifica un schema en concreto.
	/// No contiene información sobre el schema.
	class SchemaId {

	public:

		/// @brief Crea el schema id.
		/// @param name Nombre del schema.
		/// @param version Versión del schema.
		SchemaId(const std::string& name, UIndex32 version)
			: m_version(version),
			m_name(name),
			m_fullName(std::format("{}:{}", m_name, m_version)),
			m_hashCode(std::hash<std::string>::_Do_hash(m_fullName)) {

		}

		/// @return Nombre del schema.
		std::string_view GetName() const { return m_name; }

		/// @return Versión del schema.
		UIndex32 GetVersion() const { return m_version; }

		/// @return Código único del schema.
		USize64 GetHashCode() const { return m_hashCode; }

	private:

		UIndex32 m_version;
		std::string m_name;
		std::string m_fullName; // m_name:m_version
		USize64 m_hashCode;

	};

}
