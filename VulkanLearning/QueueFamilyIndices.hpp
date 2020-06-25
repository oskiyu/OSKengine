#pragma once

#include <cstdint>
#include <optional>

//Representa una cola.
struct QueueFamilyIndices {
	//Cola gráfica.
	std::optional<uint32_t> GraphicsFamily;

	//Cola de presentación.
	//(Pueden no ser la misma).
	std::optional<uint32_t> PresentFamily;

	//¿Hay alguna cola?
	bool IsComplete() const {
		return GraphicsFamily.has_value() & PresentFamily.has_value();
	}
};
