#include "StaticMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include "ModelLoadingExceptions.h"

#include <limits>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;


void StaticMeshLoader::Load(const CpuModel3D& model, GpuModel3D* output) {
	USize32 indexCount = 0;

	DynamicArray<Vertex3D> vertices{};
	DynamicArray<TIndexSize> indices{};


	TIndexSize meshesOffset = 0;
	for (const auto& mesh : model.GetMeshes()) {
		for (const auto& vertex : mesh.GetVertices()) {
			OSK_ASSERT(vertex.position.has_value(), NoVertexPositionsFoundException())
			OSK_ASSERT(vertex.normal.has_value(), NoVertexNormalsFoundException())
			OSK_ASSERT(vertex.tangent.has_value(), NoVertexTangentsFoundException())
			OSK_ASSERT(vertex.uv.has_value(), NoVertexTexCoordsFoundException())
			// OSK_ASSERT(vertex.color.has_value(), NoVertexColorFoundException())

			Vertex3D gpuVertex{};
			gpuVertex.position = vertex.position.value();
			gpuVertex.normal = vertex.normal.value();
			gpuVertex.tangent = vertex.tangent.value();
			gpuVertex.texCoords = vertex.uv.value();
			gpuVertex.color = vertex.color.value();

			vertices.Insert(gpuVertex);

			output->GetVertexAttributesMap().AddVertexAttribute(VertexPositionAttribute3D{ gpuVertex.position });
			output->GetVertexAttributesMap().AddVertexAttribute(VertexAttributes3D{ 
				gpuVertex.normal,
				gpuVertex.color,
				gpuVertex.texCoords,
				gpuVertex.tangent
				});
		}

		for (const auto& triangles : mesh.GetTriangles()) {
			indices.Insert(static_cast<TIndexSize>(triangles[0] + meshesOffset));
			indices.Insert(static_cast<TIndexSize>(triangles[1] + meshesOffset));
			indices.Insert(static_cast<TIndexSize>(triangles[2] + meshesOffset));
		}

		meshesOffset = static_cast<TIndexSize>(vertices.GetSize());
	}

	for (const auto& mesh : model.GetMeshes()) {
		const auto numMeshIndices = mesh.GetTriangleCount() * 3;

		Vector3f center = Vector3f::Zero;
		for (const auto& vertex : mesh.GetVertices()) {
			center += vertex.position.value();
		}
		center /= static_cast<float>(mesh.GetVertices().GetSize());

		float radius = 0.0f;
		for (const auto& vertex : mesh.GetVertices()) {
			const float distance = vertex.position.value().GetDistanceTo(center);
			radius = glm::max(distance, radius);
		}

		auto gpuMesh = GpuMesh3D(
			GpuMeshUuid(Engine::GetUuidProvider()->GenerateNewUuid()),
			indexCount, 
			numMeshIndices, 
			center,
			mesh.HasMaterial() ? mesh.GetMaterialIndex() : 0);

		gpuMesh.SetBoundingSphereRadius(radius);

		output->AddMesh(gpuMesh);

		indexCount += numMeshIndices;
	}

	output->SetCpuModel3D(model);

	output->SetVertexBuffer(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(vertices, Vertex3D::GetVertexInfo()));
	output->SetIndexBuffer(Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(indices));
	output->SetIndexCount(static_cast<USize32>(indices.GetSize()));
}

//			m_meshIdToMaterialId[static_cast<UIndex32>(m_meshes.GetSize())] = primitive.material;
