#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "MaterialBinding.h"
#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"

#include "ArrayStack.h"
#include <array>

namespace OSK {

	class RenderAPI;


	constexpr uint32_t MaterialPoolSize = 32;

	//Contiene los vulkan descriptor pools con un tamaño de <MaxSize>.
	struct OSKAPI_CALL MaterialPoolData {

		MaterialPoolData(RenderAPI* renderer);
		~MaterialPoolData();
		void Free();

		void SetLayout(DescriptorLayout* layout);

		DescriptorPool* DPool = nullptr;

		uint32_t CurrentSize = 0;
		uint32_t BaseSize = 0;

		//True si el pool está lleno.
		bool IsFull() const;

		uint32_t GetNextDescriptorSet();

		//Espacios sin asignar.
		ArrayStack<uint32_t> FreeSpaces;
		std::vector<DescriptorSet*> DescriptorSets;
		RenderAPI* Renderer = nullptr;

	};

}