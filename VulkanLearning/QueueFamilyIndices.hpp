#pragma once

#include <cstdint>
#include <optional>

//Representa una cola.
struct QueueFamilyIndices {
	//Cola gr�fica.
	std::optional<uint32_t> GraphicsFamily;

	//Cola de presentaci�n.
	//(Pueden no ser la misma).
	std::optional<uint32_t> PresentFamily;

	//�Hay alguna cola?
	bool IsComplete() const {
		return GraphicsFamily.has_value() & PresentFamily.has_value();
	}
};
