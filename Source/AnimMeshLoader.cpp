#include "AnimMeshLoader.h"

#include "OSKengine.h"
#include "IRenderer.h"
#include "IGpuMemoryAllocator.h"
#include "GpuImageDimensions.h"
#include "Format.h"
#include "GpuMemoryTypes.h"
#include "GpuImageLayout.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Animation.h"
#include "AnimationSampler.h"
#include "Material.h"
#include "AnimationSkin.h"

#include "ModelLoadingExceptions.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


using namespace OSK;
using namespace OSK::ASSETS;
using namespace OSK::GRAPHICS;

void AnimMeshLoader::Load(const CpuModel3D& model, GRAPHICS::GpuModel3D* output) {
	TIndexSize indexCount = 0;

	DynamicArray<VertexAnim3D> vertices{};
	DynamicArray<TIndexSize> indices{};


	for (const auto& mesh : model.GetMeshes()) {
		for (const auto& vertex : mesh.GetVertices()) {
			OSK_ASSERT(vertex.position.has_value(), NoVertexPositionsFoundException())
			OSK_ASSERT(vertex.normal.has_value(), NoVertexNormalsFoundException())
			OSK_ASSERT(vertex.tangent.has_value(), NoVertexTangentsFoundException())
			OSK_ASSERT(vertex.uv.has_value(), NoVertexTexCoordsFoundException())
			OSK_ASSERT(vertex.color.has_value(), NoVertexColorFoundException())
			OSK_ASSERT(vertex.boneIds.has_value(), NoVertexBoneIndicesFoundException())
			OSK_ASSERT(vertex.boneWeights.has_value(), NoVertexBoneWeightsFoundException())

			VertexAnim3D gpuVertex{};
			gpuVertex.position = vertex.position.value();
			gpuVertex.normal = vertex.normal.value();
			gpuVertex.tangent = vertex.tangent.value();
			gpuVertex.texCoords = vertex.uv.value();
			gpuVertex.color = vertex.color.value();
			gpuVertex.boneIndices = { vertex.boneIds.value()[0], vertex.boneIds.value()[1], vertex.boneIds.value()[2], vertex.boneIds.value()[3] };
			gpuVertex.boneWeights = { vertex.boneWeights.value()[0], vertex.boneWeights.value()[1], vertex.boneWeights.value()[2], vertex.boneWeights.value()[3] };

			vertices.Insert(gpuVertex);


			output->GetVertexAttributesMap().AddVertexAttribute(VertexPositionAttribute3D{ gpuVertex.position });
			output->GetVertexAttributesMap().AddVertexAttribute(VertexAttributes3D{
				gpuVertex.normal,
				gpuVertex.color,
				gpuVertex.texCoords,
				gpuVertex.tangent
				});
			output->GetVertexAttributesMap().AddVertexAttribute(VertexAnimationAttributes3D{ gpuVertex.boneIndices, gpuVertex.boneWeights });
		}

		for (const auto& triangles : mesh.GetTriangles()) {
			indices.Insert(static_cast<TIndexSize>(triangles[0]));
			indices.Insert(static_cast<TIndexSize>(triangles[1]));
			indices.Insert(static_cast<TIndexSize>(triangles[2]));
		}
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
			mesh.GetMaterialIndex());

		gpuMesh.SetBoundingSphereRadius(radius);

		output->AddMesh(gpuMesh);

		indexCount += numMeshIndices;
	}

	output->SetCpuModel3D(model);
	
	output->SetVertexBuffer(Engine::GetRenderer()->GetAllocator()->CreateVertexBuffer(
		vertices, 
		Vertex3D::GetVertexInfo(),
		GpuQueueType::ASYNC_TRANSFER,
		GpuQueueType::MAIN));

	output->SetIndexBuffer(Engine::GetRenderer()->GetAllocator()->CreateIndexBuffer(
		indices,
		GpuQueueType::ASYNC_TRANSFER,
		GpuQueueType::MAIN));

	output->SetIndexCount(static_cast<USize32>(indices.GetSize()));
}
