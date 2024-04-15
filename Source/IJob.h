#pragma once

#include "OSKmacros.h"

#include <string>
#include <concepts>
#include <set>


namespace OSK {

	/// @brief Representa una unidad de ejecución
	/// que puede ser ejecutada por el sistema de trabajos.
	/// 
	/// Un trabajo puede generar otros trabajos.
	class OSKAPI_CALL IJob {

	public:

		virtual ~IJob() = default;

		/// @brief Ejecuta la función del trabajo.
		virtual void Execute() = 0;

		/// @return Nombre del trabajo.
		virtual std::string_view GetName() const = 0;


		/// @brief Añade un tag al trabajo, para poder identificarlo.
		/// @param tag Tag.
		void AddTag(const std::string& tag);

		/// @return Lista con todos los tags.
		/// Puede estar vacía.
		const std::set<std::string, std::less<>>& GetTags() const;

	private:

		std::set<std::string, std::less<>> m_tags;

	};

	template<typename TJob>
	concept IsJob = requires (TJob) {
		{ TJob::GetTypeName() };
	} && std::is_base_of_v<IJob, TJob>;

}

#define OSK_JOB(typeName) constexpr static inline std::string_view GetTypeName() { return typeName; } \
std::string_view GetName() const override { return typeName; }
