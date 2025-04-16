#pragma once

#include "ApiCall.h"

#include "UniquePtr.hpp"
#include "MaterialLayout.h"
#include "HashMap.hpp"

namespace OSK::GRAPHICS {

	class MaterialLayout;
	class SpirvReflectionData;

	/// @brief Clase auxiliar para la construcción de
	/// layouts de material.
	class OSKAPI_CALL MaterialLayoutBuilder {

	public:

		/// @brief Inicializa el builder.
		/// @param slotNames Nombres de los slots.
		/// @param materialName Nombre del material.
		explicit MaterialLayoutBuilder(
			const std::unordered_map<UIndex32, std::string>& slotNames,
			const std::string& materialName);
		
		/// @brief Añade los datos de un shader del material.
		/// @param shaderData Datos del shader.
		void Apply(const SpirvReflectionData& shaderData);

		/// @return Instancia construida.
		/// @post El builder estará vacío.
		[[nodiscard]] UniquePtr<MaterialLayout> Build();

	private:

		UniquePtr<MaterialLayout> m_target{};

		std::unordered_map<UIndex32, std::string> m_slotNames;

		USize32 pushConstantsOffset = 0;

		USize32 numHlslBuffers = 0;
		USize32 numHlslImages = 0;
		USize32 numHlslBindings = 0;

	};

}
