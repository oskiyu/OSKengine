#pragma once

#include <glm.hpp>

//Estructura que almacena información por objeto que se renderiza.
struct PushConst {
	//Matrices.
	glm::mat4 Model = glm::mat4(1.0f);
};