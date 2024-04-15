#include "QueueFamilyIndices.h"

using namespace OSK;
using namespace OSK::EFTraits;
using namespace OSK::GRAPHICS;

bool QueueFamiles::IsComplete() const {
	return !GetFamilies(CommandsSupport::GRAPHICS).IsEmpty()
		&& !GetFamilies(CommandsSupport::COMPUTE).IsEmpty()
		&& !GetFamilies(CommandsSupport::TRANSFER).IsEmpty()
		&& !GetFamilies(CommandsSupport::PRESENTATION).IsEmpty();
}

DynamicArray<QueueFamily> QueueFamiles::GetFamilies(CommandsSupport support) const {
	DynamicArray<QueueFamily> output{};

	for (const auto& family : families) {
		if (EFTraits::HasFlag(family.support, support)) {
			output.Insert(family);
		}
	}

	return output;
}
