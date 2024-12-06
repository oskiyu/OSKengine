#pragma once

#include "ApiCall.h"

#include "HashMap.hpp"
#include "DynamicArray.hpp"

#include "MaterialLayoutSlot.h"
#include "ShaderBindingType.h"
#include "MaterialLayoutPushConstant.h"

#include <span>


namespace OSK::IO {
	class ILogger;
}

namespace OSK::GRAPHICS {

	enum class ShaderStage;

	/// @brief Clase con información sobre un shader.
	class OSKAPI_CALL SpirvReflectionData {

	public:

		explicit SpirvReflectionData(std::span<const char> bytecode, ShaderStage stage);
		explicit SpirvReflectionData(const DynamicArray<char>& bytecode, ShaderStage stage);

		/// @return Material slots del shader.
		/// Índice -> slot.
		const std::unordered_map<UIndex64, MaterialLayoutSlot>& GetSlots() const;

		/// @return Push constants del shader.
		std::span<const MaterialLayoutPushConstant> GetPushConstants() const;

		/// @return Stage del shader.
		ShaderStage GetShaderStage() const;


		/// @brief Muestra los contenidos del shader
		/// por el logger.
		/// @param logger Logger a usar.
		void Log(IO::ILogger* logger) const;

	private:

		std::unordered_map<UIndex64, MaterialLayoutSlot> m_slots;
		DynamicArray<MaterialLayoutPushConstant> m_pushConstants;

		ShaderStage m_stage{};

	};

}
