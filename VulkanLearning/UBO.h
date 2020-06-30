#pragma once

#include <glm.hpp>

//Estructura que almacena información que se actualiza una vez por frame.
struct UniformBufferObject {
	//Variables de la cámara.
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
};
