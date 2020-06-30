#pragma once

#include <glm.hpp>

//Estructura que almacena informaci�n que se actualiza una vez por frame.
struct UniformBufferObject {
	//Variables de la c�mara.
	alignas(16) glm::mat4 View;
	alignas(16) glm::mat4 Projection;
};
