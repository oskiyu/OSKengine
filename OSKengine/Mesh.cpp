#include "Mesh.h"

namespace OSK {

	Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices) {
		Vertices = vertices;
		Indices = indices;

		setupMesh();
	}


	Mesh::~Mesh() {

	}
	

	void Mesh::setupMesh() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		
		//glEnableVertexAttribArray(3);
		//glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, sizeof(int32_t), (void*)offsetof(Vertex, BoneID));

		for (uint32_t i = 0; i < OSK::__VERTEX::MAX_BONES_AMOUNT; i++) {
			glVertexAttribPointer(3 + i, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, BoneID) + sizeof(float_t) * i));
			glEnableVertexAttribArray(3 + i);
		}

		for (uint32_t i = 0; i < OSK::__VERTEX::MAX_BONES_AMOUNT; i++) {
			glVertexAttribPointer(3 + 6 + i, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, BoneWeights) + sizeof(float_t) * i));
			glEnableVertexAttribArray(3 + 6 + i);
		}

		glBindVertexArray(0);
	}

}