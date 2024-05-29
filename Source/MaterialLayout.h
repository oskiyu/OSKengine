#pragma once

#include "MaterialLayoutSlot.h"
#include "ApiCall.h"
#include "LinkedList.hpp"
#include "MaterialLayoutPushConstant.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// El layout de un material determina qué slots pertenecen a ese material.
	/// </summary>
	class OSKAPI_CALL MaterialLayout {

	public:

		using PushConstantsContainer = std::unordered_map<std::string, MaterialLayoutPushConstant, StringHasher, std::equal_to<>>;
		using PushConstantsIterable = PushConstantsContainer;

		using MaterialSlotsContainer = std::unordered_map<std::string, MaterialLayoutSlot, StringHasher, std::equal_to<>>;
		using MaterialSlotsIterable = MaterialSlotsContainer;

		/// <summary>
		/// Añade el slot dado al layout.
		/// El nombre del slot se usará como llave.
		/// 
		/// @warning No pueden haber dos slots con el mismo nombre.
		/// </summary>
		void AddSlot(const MaterialLayoutSlot& slot);

		/// <summary>
		/// Registra un slot para un push constant.
		/// </summary>
		void AddPushConstant(MaterialLayoutPushConstant pushConstant);

		/// <summary>
		/// Obtiene el slot con el nombre dado, si existe.
		/// 
		/// @warning No se hace comprobación de que el slot existe.
		/// </summary>
		const MaterialLayoutSlot& GetSlot(std::string_view name) const;

		/// <summary>
		/// Obtiene el slot con el nombre dado, si existe.
		/// 
		/// @warning No se hace comprobación de que el slot existe.
		/// </summary>
		MaterialLayoutSlot& GetSlot(std::string_view name);

		/// <summary>
		/// Devuelve el slot push constant con el nombre dado.
		/// 
		/// @warning No se hace comprobación de que el slot existe.
		/// </summary>
		const MaterialLayoutPushConstant& GetPushConstant(std::string_view name) const;

		/// <summary>
		/// Devuelve el slot push constant con el nombre dado.
		/// 
		/// @warning No se hace comprobación de que el slot existe.
		/// </summary>
		MaterialLayoutPushConstant& GetPushConstant(std::string_view name);

		/// <summary>
		/// Obtiene los slots.
		/// </summary>
		const MaterialSlotsIterable& GetAllSlots() const;

		/// <summary>
		/// Devuelve todos los slots push constants registrados.
		/// </summary>
		const PushConstantsIterable& GetAllPushConstants() const;

		/// <summary>
		/// Obtiene los slots.
		/// </summary>
		MaterialSlotsIterable& GetAllSlots();

		/// <summary>
		/// Devuelve todos los slots push constants registrados.
		/// </summary>
		PushConstantsIterable& GetAllPushConstants();

	private:

		std::string m_name;
		std::unordered_map<std::string, MaterialLayoutSlot, StringHasher, std::equal_to<>> m_slots;
		std::unordered_map<std::string, MaterialLayoutPushConstant, StringHasher, std::equal_to<>> m_pushConstants;
		USize32 m_accumulatedPushConstOffset = 0;

	};

}
