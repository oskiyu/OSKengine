#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "MaterialBinding.h"
#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"

#include "Stack.hpp"
#include <array>

namespace OSK {

	class RenderAPI;

	/// <summary>
	/// Número de descriptor sets en un material pool data.
	/// </summary>
	constexpr uint32_t MaterialPoolSize = 32;

	/// <summary>
	/// Contiene los vulkan descriptor pools con un tamaño de <MaxSize>.
	/// </summary>
	struct OSKAPI_CALL MaterialPoolData {

		/// <summary>
		/// Crea el data.
		/// </summary>
		/// <param name="renderer">Renderizador.</param>
		MaterialPoolData(RenderAPI* renderer, uint32_t baseSize);

		/// <summary>
		/// Destruye el data.
		/// Elimina el descriptor pool.
		/// </summary>
		~MaterialPoolData();

		/// <summary>
		/// Destruye el data.
		/// Elimina el descriptor pool.
		/// </summary>
		void Free();

		/// <summary>
		/// Establece el layout del material.
		/// </summary>
		/// <param name="layout">Layout.</param>
		void SetLayout(DescriptorLayout* layout);

		/// <summary>
		/// True si el pool está lleno.
		/// </summary>
		/// <returns>Estado del pool.</returns>
		bool IsFull() const;

		/// <summary>
		/// Establece los bindings.
		/// </summary>
		void SetBindings(const std::vector<VkDescriptorType>& bindings);

		/// <summary>
		/// Devuelve el descriptor set localizado en la posición dada (local).
		/// </summary>
		DescriptorSet* GetDescriptorSet(uint32_t localIndex);

		/// <summary>
		/// Libera un descriptor set.
		/// </summary>
		void FreeDescriptorSet(uint32_t localIndex);

		/// <summary>
		/// Devuelve el ID del próximo descriptor set vacío.
		/// </summary>
		/// <returns>ID.</returns>
		uint32_t GetNextDescriptorSet();

	private:

		/// <summary>
		/// Descriptor pool.
		/// </summary>
		DescriptorPool* descriptorPool = nullptr;

		/// <summary>
		/// Descriptor set más alto que ha sido utilizado.
		/// </summary>
		uint32_t currentSize = 0;

		/// <summary>
		/// ID del primer descriptor set del data.
		/// 0 - 32 - 64 ...
		/// </summary>
		uint32_t baseSize = 0;

		/// <summary>
		/// Espacios sin asignar.
		/// </summary>
		ArrayStack<uint32_t> freeSpaces;

		/// <summary>
		/// Descriptor sets del pool
		/// </summary>
		std::vector<DescriptorSet*> descriptorSets;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* renderer = nullptr;

	};

}