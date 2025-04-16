#pragma once

#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "Model3D.h"
#include "Color.hpp"
#include "Vertex.h"

#include "PbrMaterialInfo.h"
#include "Animation.h"
#include "AnimationSampler.h"
#include "AnimationSkin.h"
#include "Bone.h"

#include "Animation.h"
#include "AnimationSkin.h"

#include "CpuModel3D.h"
#include "GpuModel3D.h"

#include "EnumFlags.hpp"


namespace tinygltf {
	class Model;
	struct Mesh;
	struct Primitive;
	class Node;
	struct Scene;
	struct Animation;
}


namespace OSK::GRAPHICS {
	class GpuImage;
}

namespace OSK::ASSETS {


	/// @brief Clase base para poder importar un modelo GLTF.
	/// 
	/// Incluye métodos para importar información relevante del
	/// modelo GLTF.
	class OSKAPI_CALL GltfLoader {

	public:

		static CpuModel3D Load(
			std::string_view path,
			const glm::mat4& transform);

		static void LoadMaterials(
			std::string_view path,
			DynamicArray<GRAPHICS::GpuModel3D::Material>* materials,
			GRAPHICS::GpuModel3D::TextureTable* textures);

	protected:

		static DynamicArray<GRAPHICS::Animation> LoadAnimations(
			const tinygltf::Model& model, 
			const glm::mat4& baseTransform);

		static DynamicArray<GRAPHICS::AnimationSampler> LoadAnimationSamplers(
			const tinygltf::Model& model,
			const tinygltf::Animation& gltfAnimation);
		static DynamicArray<GRAPHICS::AnimationChannel> LoadAnimationChannels(const tinygltf::Animation& gltfAnimation);
		
		static DynamicArray<GRAPHICS::AnimationSkin> LoadAnimationSkins(const tinygltf::Model& model);

		static DynamicArray<std::string> GetTagsFromName(const tinygltf::Scene& scene);

		static DynamicArray<GRAPHICS::GpuModel3D::Material> LoadMaterials(const tinygltf::Model& model);

		/// @brief Devuelve una matriz en formato GLM con el transform del nodo.
		/// @param node Nodo GLTF.
		/// @return Matriz con su transform.
		static glm::mat4 GetNodeMatrix(const tinygltf::Node& node);

		
		/// @brief Procesa un nodo de un modelo GLTF.
		/// Este nodo puede contener o no geometría 3D en forma
		/// de meshes.
		/// @param node Nodo GLTF.
		/// @param nodeId ID del nodo procesado.
		/// @param parentId ID del nodo padre (TSize::max si no tiene padre).
		static DynamicArray<CpuMesh3D> ProcessNode(
			const tinygltf::Model& model,
			const tinygltf::Node& node,
			const glm::mat4& transform);

		static DynamicArray<GRAPHICS::AnimationBone> LoadBones(
			const tinygltf::Model& model,
			const tinygltf::Node& node,
			UIndex64 nodeIndex,
			std::optional<UIndex64> parentIndex);

		static DynamicArray<GRAPHICS::AnimationBone> LoadAllBones(
			const tinygltf::Model& model,
			const glm::mat4& baseTransform);

		/// @brief Comprueba si la primitiva GLTF dada contiene un atributo
		/// en específico.
		/// @param primitive Primitiva GLTF.
		/// @param name Nombre del atributo, según la especificación.
		/// Por ejempl, "POSITION" o "COLOR_0".
		/// @return True si lo tiene, false en caso contrario.
		/// 
		/// @pre @p name debe referirse a un atributo válido GLTF.
		/// 
		/// @note Si el atributo dado por @p name no se corresponde con ninguno
		/// de los atributos posibles, devuelve false. 
		static bool HasAttribute(const tinygltf::Primitive& primitive, const std::string& name);

		/// @brief Comrpueba si la primitiva contiene la posición de sus vértices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene posiciones, false en caso contrario.
		static bool HasPositions(const tinygltf::Primitive& primitive);

		/// @brief Comrpueba si la primitiva contiene los vectores normales de sus vértices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene normales, false en caso contrario.
		static bool HasNormals(const tinygltf::Primitive& primitive);

		/// @brief Comrpueba si la primitiva contiene los vectores tangenciales de sus vértices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene tangentes, false en caso contrario.
		static bool HasTangets(const tinygltf::Primitive& primitive);

		/// @brief Comrpueba si la primitiva contiene las coordenadas de texturas de sus vértices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene coordenadas de texturas, false en caso contrario.
		static bool HasTextureCoords(const tinygltf::Primitive& primitive);

		/// @brief Comrpueba si la primitiva contiene los colores de sus vértices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene colores, false en caso contrario.
		static bool HasColors(const tinygltf::Primitive& primitive);

		/// @brief Comrpueba si la primitiva contiene los joints (para animación esqueletal) de sus vértices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene joints, false en caso contrario.
		static bool HasJoints(const tinygltf::Primitive& primitive);

		/// @brief Comrpueba si la primitiva contiene los pesos de animación esqueletal de sus vértices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene pesos, false en caso contrario.
		static bool HasBoneWeights(const tinygltf::Primitive& primitive);


		/// @brief Obtiene las posiciones de todos los vértices de la primitiva.
		/// @param primitive Primitiva GLTF.
		/// @param nodeMatrix Transform local del nodo que está siendo procesado.
		/// @return Lista con todas las posiciones 3D.
		/// 
		/// @pre HasPositions() debe ser true para la primitiva dada por @p primitive.
		/// 
		/// @throws std::runtime_error si se incumple la precondición.
		/// 
		/// @note Hay exactamente una posición por cada vértice de la primitiva.
		/// @note Cada posición se asocia con cada vértice con el mismo índice dentro de la lista
		/// (vertices[0].pos = GetVertexPositions()[0], etc.).
		static DynamicArray<Vector3f> GetVertexPositions(
			const tinygltf::Primitive& primitive, 
			const glm::mat4& nodeMatrix,
			const tinygltf::Model& model);
		
		/// @brief Obtiene las normales de todos los vértices de la primitiva.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con todas las normales de la primitiva.
		/// 
		/// @pre HasNormals() debe ser true para la primitiva dada por @p primitive.
		/// 
		/// @throws std::runtime_error si se incumple la precondición.
		/// 
		/// @note Hay exactamente una normal por cada vértice de la primitiva.
		/// @note Cada normal se asocia con cada vértice con el mismo índice dentro de la lista
		/// (vertices[0].normal = GetVertexNormals()[0], etc.).
		static DynamicArray<Vector3f> GetVertexNormals(
			const tinygltf::Primitive& primitive,
			const tinygltf::Model& model);

		/// @brief Obtiene las coordenadas de texturas de todos los vértices de la primitiva.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con todas las coordenadas de texturas de la primitiva.
		/// 
		/// @pre HasTextureCoords() debe ser true para la primitiva dada por @p primitive.
		/// 
		/// @throws std::runtime_error si se incumple la precondición.
		/// 
		/// @note Hay exactamente una coordenada por cada vértice de la primitiva.
		/// @note Cada normal se asocia con cada vértice con el mismo índice dentro de la lista
		/// (vertices[0].texCoords = GetTextureCoords()[0], etc.).
		static DynamicArray<Vector2f> GetTextureCoords(
			const tinygltf::Primitive& primitive,
			const tinygltf::Model& model);

		/// @brief Obtiene los colores de todos los vértices de la primitiva, si tienen.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con los colores de los vértices de la primitiva (puede estar vacía).
		/// 
		/// @note Si HasColors() para la primitiva dada por @p primitive es false, entonces devuelve una lista vacía.
		/// @note Si la lista no está vacía, hay exactamente un color por cada vértice de la primitiva.
		/// @note Si la lista no está vacía, cada color se asocia con cada vértice con el mismo índice dentro de la lista
		/// (vertices[0].color = GetVertexColors()[0], etc.).
		static DynamicArray<Color> GetVertexColors(
			const tinygltf::Primitive& primitive,
			const tinygltf::Model& model);

		/// @brief Obtiene los joints de animación esqueletal de todos los vértices de la primitiva, si tienen.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con los joints de los vértices de la primitiva (puede estar vacía).
		/// 
		/// @note Si HasJoints() para la primitiva dada por @p primitive es false, entonces devuelve una lista vacía.
		/// @note Si la lista no está vacía, hay exactamente un joint (Vector4f) por cada vértice de la primitiva.
		/// @note Si la lista no está vacía, cada joint se asocia con cada vértice con el mismo índice dentro de la lista
		/// (vertices[0].joints = GetJoints()[0], etc.).
		static DynamicArray<Vector4f> GetJoints(
			const tinygltf::Primitive& primitive,
			const tinygltf::Model& model);

		/// @brief Obtiene los pesos de animación esqueletal de todos los vértices de la primitiva, si tienen.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con los pesos de los vértices de la primitiva (puede estar vacía).
		/// 
		/// @note Si HasBoneWeights() para la primitiva dada por @p primitive es false, entonces devuelve una lista vacía.
		/// @note Si la lista no está vacía, hay exactamente un peso (Vector4f) por cada vértice de la primitiva.
		/// @note Si la lista no está vacía, cada peso se asocia con cada vértice con el mismo índice dentro de la lista
		/// (vertices[0].weights = GetJoints()[0], etc.).
		static DynamicArray<Vector4f> GetBoneWeights(
			const tinygltf::Primitive& primitive,
			const tinygltf::Model& model);

		/// @param primitive Mesh.
		/// @param startOffset Índice del primer índice.
		/// @return Lista con los índices de @p primitive.
		static DynamicArray<GRAPHICS::TIndexSize> GetIndices(
			const tinygltf::Primitive& primitive, 
			GRAPHICS::TIndexSize startOffset,
			const tinygltf::Model& model);

		/// @param primitive Mesh.
		/// @return Vectores tangentes de los vértices del mesh.
		/// @pre El mesh debe tener vectores tangentes preprocesados.
		/// @throws NoVertexTangentsFoundException si no se cumple la precondición.
		static DynamicArray<Vector3f> GetTangentVectors(
			const tinygltf::Primitive& primitive,
			const tinygltf::Model& model);

		/// @brief Genera vectores tangentes para los vértices indicados.
		/// @param texCoords Coordenadas de texturas UV de los vértices.
		/// @param positions Posiciones de los vértices.
		/// @param indices Índices de los vértices.
		/// @param indicesStartOffset Primer índice de los vértices.
		/// @return Vectores tangentes.
		/// 
		/// @pre @p texCoords.GetSize() == @p positions.GetSize()
		/// @pre @p indices.GetSize() debe ser múltiplo de 3.
		/// @param indicesStartOffset debe ser múltiplo de 3.
		static DynamicArray<Vector3f> GenerateTangetVectors(
			const DynamicArray<Vector2f>& texCoords,
			const DynamicArray<Vector3f>& positions,
			const DynamicArray<GRAPHICS::TIndexSize>& indices,
			UIndex32 indicesStartOffset);

		/// @param mesh Mesh.
		/// @return Propiedades extra del mesh.
		static std::unordered_map<std::string, std::string, StringHasher, std::equal_to<>> GetCustomProperties(const tinygltf::Mesh& mesh);


		/// @return Todas las texturas usadas en el modelo 3D.
		static DynamicArray<ASSETS::AssetRef<ASSETS::Texture>> LoadImages(const tinygltf::Model& model);

	};

}
