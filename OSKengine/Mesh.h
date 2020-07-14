#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "Shader.h"
#include "OldVertex.h"

#include <vector>

namespace OSK {

	//Almacena una malla.
	class OSKAPI_CALL Mesh {

	public:

		//Crea una malla con los vértices y los índeces proporcionados.
		//Las mallas pertenecen a un modelo 3D (OSK::Model).
		//Son creadas por un ContentAPI, al cargar un modelo 3D.
		OSK_INFO_DO_NOT_TOUCH
			Mesh(const std::vector<OldVertex>&, const std::vector<vertexIndex_t>&);


		//Destruye la malla.
		~Mesh();


		//Vértices de la malla.
		std::vector<OldVertex> Vertices;


		//Índices de los vértices de la pantalla.
		std::vector<vertexIndex_t> Indices;


		//True si la malla usa animaciones.
		bool Skeletal = false;


		//OpenGL
		OSK_INFO_DO_NOT_TOUCH
			vertexArrayObject_t VAO;


		//OpenGL
		OSK_INFO_DO_NOT_TOUCH
			bufferObject_t VBO;


		//OpenGL
		OSK_INFO_DO_NOT_TOUCH
			bufferObject_t EBO;

	private:

		void setupMesh();

	};

}