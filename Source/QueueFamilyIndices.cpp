#include "QueueFamilyIndices.h"

using namespace OSK;
using namespace OSK::EFTraits;
using namespace OSK::GRAPHICS;

bool QueueFamiles::IsComplete() const {
	return !GetFamilies(CommandQueueSupport::GRAPHICS).IsEmpty()
		&& !GetFamilies(CommandQueueSupport::COMPUTE).IsEmpty()
		&& !GetFamilies(CommandQueueSupport::TRANSFER).IsEmpty()
		&& !GetFamilies(CommandQueueSupport::PRESENTATION).IsEmpty();
}

DynamicArray<QueueFamily> QueueFamiles::GetFamilies(CommandQueueSupport support) const {
	DynamicArray<QueueFamily> output{};

	for (const auto& family : families)
		if ((family.support & support) == support)
			output.Insert(family);

	return output;
}
