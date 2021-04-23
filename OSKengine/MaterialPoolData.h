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
		MaterialPoolData(RenderAPI* renderer);

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
		/// Descriptor pool.
		/// </summary>
		DescriptorPool* DPool = nullptr;

		/// <summary>
		/// Descriptor set más alto que ha sido utilizado.
		/// </summary>
		uint32_t CurrentSize = 0;

		/// <summary>
		/// ID del primer descriptor set del data.
		/// 0 - 32 - 64 ...
		/// </summary>
		uint32_t BaseSize = 0;

		/// <summary>
		/// True si el pool está lleno.
		/// </summary>
		/// <returns>Estado del pool.</returns>
		bool IsFull() const;

		/// <summary>
		/// Devuelve el ID del próximo descriptor set vacío.
		/// </summary>
		/// <returns>ID.</returns>
		uint32_t GetNextDescriptorSet();

		/// <summary>
		/// Espacios sin asignar.
		/// </summary>
		ArrayStack<uint32_t> FreeSpaces;

		/// <summary>
		/// Descriptor sets del pool
		/// </summary>
		std::vector<DescriptorSet*> DescriptorSets;

		/// <summary>
		/// Renderizador.
		/// </summary>
		RenderAPI* Renderer = nullptr;

	};

}