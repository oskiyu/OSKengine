#include "ModelLoader3D.h"

#include "Model3D.h"
#include "DynamicArray.hpp"
#include "Vertex.h"
#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuVertexBuffer.h"
#include "IGpuIndexBuffer.h"
#include "IGpuMemoryAllocator.h"
#include "FileIO.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <gtc/type_ptr.hpp>
#include <json.hpp>

using namespace OSK;

constexpr auto ASSIMP_FLAGS = aiProcess_Triangulate | aiProcess_GenNormals;

glm::mat4 AiToGLM(const aiMatrix4x4& from) {
	return glm::transpose(glm::make_mat4(&from.a1));
}

void ProcessMeshNode(const aiNode* node, const aiScene* scene, DynamicArray<Vertex3D>* vertices, DynamicArray<TIndexSize>* indices, const Vector3f& prevPosition, float globalScale) {
	aiVector3D aiposition;
	aiQuaternion airotation;
	aiVector3D aiscale;

	node->mTransformation.Decompose(aiscale, airotation, aiposition);

	auto matrix = AiToGLM(node->mTransformation);
	Vector3f position = prevPosition + glm::vec3(matrix[3]);

	for (TSize i = 0; i < node->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		for (TSize v = 0; v < mesh->mNumVertices; v++) {
			Vertex3D vertex{};
			Vector3f vec3 = glm::make_vec3(&mesh->mVertices[v].x) * globalScale;

			vertex.position = vec3 + position.ToGLM();

			//vertex.normals.x = mesh->mNormals[v].x;
			//vertex.normals.y = -mesh->mNormals[v].y;
			//vertex.normals.z = mesh->mNormals[v].z;

			vertex.texCoords.X = mesh->mTextureCoords[0][v].x;
			vertex.texCoords.Y = -mesh->mTextureCoords[0][v].y;

			if (mesh->HasVertexColors(0))
				vertex.color = Color(mesh->mColors[0][v].r,
									 mesh->mColors[0][v].g,
									 mesh->mColors[0][v].b);
			else
				vertex.color = Color::WHITE();

			vertex.position.Y *= -1.0f;

			vertices->Insert(vertex);
		}
	}

	for (TSize i = 0; i < node->mNumMeshes; i++) {
		TSize indexBase = indices->GetSize();

		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		for (TSize f = 0; f < mesh->mNumFaces; f++)
			for (TSize ind = 0; ind < 3; ind++)
				indices->Insert(mesh->mFaces[f].mIndices[ind] + indexBase);
	}

	for (TSize i = 0; i < node->mNumChildren; i++)
		ProcessMeshNode(node->mChildren[i], scene, vertices, indices, position, globalScale);
}

void ModelLoader3D::Load(const std::string& assetFilePath, IAsset** asset) {
	Model3D* output = (Model3D*)*asset;

	// a
	nlohmann::json assetInfo = nlohmann::json::parse(FileIO::ReadFromFile(assetFilePath));
	std::string texturePath = assetInfo["raw_asset_path"];
	output->SetName(assetInfo["name"]);

	OSK_ASSERT(FileIO::FileExists(assetInfo["raw_asset_path"]), "El modelo en no existe.");

	static Assimp::Importer assimpImporter;

	// Escena de Assimp.
	const aiScene* scene = nullptr;

	scene = assimpImporter.ReadFile(assetInfo["raw_asset_path"], ASSIMP_FLAGS);

	DynamicArray<Vertex3D> vertices;
	DynamicArray<TIndexSize> indices;

	ProcessMeshNode(scene->mRootNode, scene, &vertices, &indices, { 0.0f }, assetInfo["scale"]);

	// GPU.
	output->_SetVertexBuffer(Engine::GetRenderer()->GetMemoryAllocator()->CreateVertexBuffer(vertices));
	output->_SetIndexBuffer(Engine::GetRenderer()->GetMemoryAllocator()->CreateIndexBuffer(indices));

	output->_SetIndexCount(vertices.GetSize());
}
