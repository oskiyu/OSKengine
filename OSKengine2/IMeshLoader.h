#pragma once

#include "DynamicArray.hpp"
#include "Mesh3D.h"
#include "HashMap.hpp"
#include "Model3D.h"
#include "Color.hpp"
#include "Vertex.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>


namespace OSK::GRAPHICS {
	class GpuImage;
}

namespace OSK::ASSETS {


	/// @brief Informaci�n relevante de un material del modelo GLTF.
	struct GltfMaterialInfo {

		/// <summary>
		/// Color base.
		/// Para los v�rtices.
		/// </summary>
		Color baseColor;

		/// <summary> ID de la textura base. </summary>
		UIndex32 baseTextureIndex = 0;

		/// <summary>
		/// Factor met�lico del material.
		/// Afecta el reflejo met�lico.
		/// </summary>
		///
		/// @note Entre 0 y 1, ambos incluidos.
		float metallicFactor = 0.0f;

		/// <summary>
		/// ID de la textura que describe la naturaleza
		/// met�lica del material.
		/// </summary>
		UIndex32 metallicTextureIndex = 0;

		/// <summary> Factor de rugosidad del material. </summary>
		///
		/// @note Entre 0 y 1, ambos incluidos.
		float roughnessFactor = 0.0f;

		/// <summary>
		/// ID de la textura que describe la rugosidad
		/// del material.
		/// </summary>
		UIndex32 roughnessTextureIndex = 0;

		/// <summary> True si tiene textura. </summary>
		bool hasBaseTexture = false;

		/// <summary> True si tiene textura met�lica. </summary>
		bool hasMetallicTexture = false;

		/// <summary> True si tiene textura de rugosidad. </summary>
		bool hasRoughnessTexture = false;

		bool hasNormalTexture = false;
		UIndex32 normalTextureIndex = 0;

	};


	/// <summary>
	/// Informaci�n relevante de un modelo GLTF,
	/// necesario para la correcta generaci�n de meshes.
	/// </summary>
	struct GltfModelInfo {

		/// <summary>
		/// Informaci�n sobre todos los materiales del modelo.
		/// ID del material = posici�n dentro del array.
		/// </summary>
		DynamicArray<GltfMaterialInfo> materialInfos;

	};


	/// @brief Clase base para poder importar un modelo GLTF.
	/// Se debe crear una clase hija por cada tipo de v�rtice
	/// que se quiera cargar.
	/// 
	/// Incluye m�todos para importar informaci�n relevante del
	/// modelo GLTF.
	/// 
	/// Por defecto hay 2: 
	/// - StaticMeshLoader para modelos 3D no animados (Vertex3D).
	/// - AnimMeshLoader para modelos 3D animados (AnimVertex3D).
	class IMeshLoader {

	public:

		/// @brief Carga el modelo 3D.
		/// @param rawAssetPath Direcci�n del archivo "*.glb".
		/// @param modelTransform Transform aplicado al modelo.
		void Load(const std::string& rawAssetPath, const glm::mat4& modelTransform);

		/// @brief Configura el modelo 3D.
		/// @param model Modelo a configurar.
		/// 
		/// @pre @p model no debe ser null.
		/// 
		/// @note Las clases derivadas deben sobreescribir esta funci�n,
		/// creando dentro el vertex buffer del modelo 3D.
		virtual void SetupModel(Model3D* model);

	protected:

		/// @brief Devuelve una matriz en formato GLM con el transform del nodo.
		/// @param node Nodo GLTF.
		/// @return Matriz con su transform.
		static glm::mat4 GetNodeMatrix(const tinygltf::Node& node);

		/// <summary>
		/// Procesa un nodo de un modelo GLTF.
		/// Este nodo puede contener o no geometr�a 3D en forma
		/// de meshes.
		/// </summary>
		/// 
		/// <param name="node">Nodo GLTF.</param>
		/// <param name="nodeId">ID del nodo procesado.</param>
		/// <param name="parentId">ID del nodo padre (TSize::max si no tiene padre).</param>
		/// 
		/// @note Debe ser sobreescrito, para poder manejar los tipos de v�rtices distintos.
		virtual void ProcessNode(const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) = 0;

		/// <summary> Establece las normales del v�rtice (si tiene). </summary>
		/// 
		/// @note Debe ser sobreescrito, para poder manejar los tipos de v�rtices distintos.
		virtual void SmoothNormals() = 0;


		/// @brief Comprueba si la primitiva GLTF dada contiene un atributo
		/// en espec�fico.
		/// @param primitive Primitiva GLTF.
		/// @param name Nombre del atributo, seg�n la especificaci�n.
		/// Por ejempl, "POSITION" o "COLOR_0".
		/// @return True si lo tiene, false en caso contrario.
		/// 
		/// @pre @p name debe referirse a un atributo v�lido GLTF.
		/// 
		/// @note Si el atributo dado por @p name no se corresponde con ninguno
		/// de los atributos posibles, devuelve false. 
		bool HasAttribute(const tinygltf::Primitive& primitive, const std::string& name) const;

		/// @brief Comrpueba si la primitiva contiene la posici�n de sus v�rtices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene posiciones, false en caso contrario.
		bool HasPositions(const tinygltf::Primitive& primitive) const;

		/// @brief Comrpueba si la primitiva contiene los vectores normales de sus v�rtices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene normales, false en caso contrario.
		bool HasNormals(const tinygltf::Primitive& primitive) const;

		/// @brief Comrpueba si la primitiva contiene los vectores tangenciales de sus v�rtices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene tangentes, false en caso contrario.
		bool HasTangets(const tinygltf::Primitive& primitive) const;

		/// @brief Comrpueba si la primitiva contiene las coordenadas de texturas de sus v�rtices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene coordenadas de texturas, false en caso contrario.
		bool HasTextureCoords(const tinygltf::Primitive& primitive) const;

		/// @brief Comrpueba si la primitiva contiene los colores de sus v�rtices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene colores, false en caso contrario.
		bool HasColors(const tinygltf::Primitive& primitive) const;

		/// @brief Comrpueba si la primitiva contiene los joints (para animaci�n esqueletal) de sus v�rtices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene joints, false en caso contrario.
		bool HasJoints(const tinygltf::Primitive& primitive) const;

		/// @brief Comrpueba si la primitiva contiene los pesos de animaci�n esqueletal de sus v�rtices.
		/// @param primitive Primitiva GLTF.
		/// @return True si tiene pesos, false en caso contrario.
		bool HasBoneWeights(const tinygltf::Primitive& primitive) const;


		/// @brief Obtiene las posiciones de todos los v�rtices de la primitiva.
		/// @param primitive Primitiva GLTF.
		/// @param nodeMatrix Transform local del nodo que est� siendo procesado.
		/// @return Lista con todas las posiciones 3D.
		/// 
		/// @pre HasPositions() debe ser true para la primitiva dada por @p primitive.
		/// 
		/// @throws std::runtime_error si se incumple la precondici�n.
		/// 
		/// @note Hay exactamente una posici�n por cada v�rtice de la primitiva.
		/// @note Cada posici�n se asocia con cada v�rtice con el mismo �ndice dentro de la lista
		/// (vertices[0].pos = GetVertexPositions()[0], etc.).
		DynamicArray<Vector3f> GetVertexPositions(const tinygltf::Primitive& primitive, const glm::mat4& nodeMatrix) const;
		
		/// @brief Obtiene las normales de todos los v�rtices de la primitiva.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con todas las normales de la primitiva.
		/// 
		/// @pre HasNormals() debe ser true para la primitiva dada por @p primitive.
		/// 
		/// @throws std::runtime_error si se incumple la precondici�n.
		/// 
		/// @note Hay exactamente una normal por cada v�rtice de la primitiva.
		/// @note Cada normal se asocia con cada v�rtice con el mismo �ndice dentro de la lista
		/// (vertices[0].normal = GetVertexNormals()[0], etc.).
		DynamicArray<Vector3f> GetVertexNormals(const tinygltf::Primitive& primitive) const;

		/// @brief Obtiene las coordenadas de texturas de todos los v�rtices de la primitiva.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con todas las coordenadas de texturas de la primitiva.
		/// 
		/// @pre HasTextureCoords() debe ser true para la primitiva dada por @p primitive.
		/// 
		/// @throws std::runtime_error si se incumple la precondici�n.
		/// 
		/// @note Hay exactamente una coordenada por cada v�rtice de la primitiva.
		/// @note Cada normal se asocia con cada v�rtice con el mismo �ndice dentro de la lista
		/// (vertices[0].texCoords = GetTextureCoords()[0], etc.).
		DynamicArray<Vector2f> GetTextureCoords(const tinygltf::Primitive& primitive) const;

		/// @brief Obtiene los colores de todos los v�rtices de la primitiva, si tienen.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con los colores de los v�rtices de la primitiva (puede estar vac�a).
		/// 
		/// @note Si HasColors() para la primitiva dada por @p primitive es false, entonces devuelve una lista vac�a.
		/// @note Si la lista no est� vac�a, hay exactamente un color por cada v�rtice de la primitiva.
		/// @note Si la lista no est� vac�a, cada color se asocia con cada v�rtice con el mismo �ndice dentro de la lista
		/// (vertices[0].color = GetVertexColors()[0], etc.).
		DynamicArray<Color> GetVertexColors(const tinygltf::Primitive& primitive) const;

		/// @brief Obtiene los joints de animaci�n esqueletal de todos los v�rtices de la primitiva, si tienen.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con los joints de los v�rtices de la primitiva (puede estar vac�a).
		/// 
		/// @note Si HasJoints() para la primitiva dada por @p primitive es false, entonces devuelve una lista vac�a.
		/// @note Si la lista no est� vac�a, hay exactamente un joint (Vector4f) por cada v�rtice de la primitiva.
		/// @note Si la lista no est� vac�a, cada joint se asocia con cada v�rtice con el mismo �ndice dentro de la lista
		/// (vertices[0].joints = GetJoints()[0], etc.).
		DynamicArray<Vector4f> GetJoints(const tinygltf::Primitive& primitive) const;

		/// @brief Obtiene los pesos de animaci�n esqueletal de todos los v�rtices de la primitiva, si tienen.
		/// @param primitive Primitiva GLTF.
		/// @return Lista con los pesos de los v�rtices de la primitiva (puede estar vac�a).
		/// 
		/// @note Si HasBoneWeights() para la primitiva dada por @p primitive es false, entonces devuelve una lista vac�a.
		/// @note Si la lista no est� vac�a, hay exactamente un peso (Vector4f) por cada v�rtice de la primitiva.
		/// @note Si la lista no est� vac�a, cada peso se asocia con cada v�rtice con el mismo �ndice dentro de la lista
		/// (vertices[0].weights = GetJoints()[0], etc.).
		DynamicArray<Vector4f> GetBoneWeights(const tinygltf::Primitive& primitive) const;

		DynamicArray<GRAPHICS::TIndexSize> GetIndices(const tinygltf::Primitive& primitive, GRAPHICS::TIndexSize startOffset) const;

		DynamicArray<Vector3f> GetTangentVectors(const tinygltf::Primitive& primitive) const;

		DynamicArray<Vector3f> GenerateTangetVectors(
			const DynamicArray<Vector2f>& texCoords,
			const DynamicArray<Vector3f>& positions,
			const DynamicArray<GRAPHICS::TIndexSize>& indices,
			UIndex32 indicesStartOffset) const;

		/// <summary> Carga todas las texturas usadas en el modelo 3D. </summary>
		DynamicArray<OwnedPtr<GRAPHICS::GpuImage>> LoadImages();

		/// <summary> Carga todos los materiales usados en el modelo 3D. </summary>
		DynamicArray<GltfMaterialInfo> LoadMaterials();

		DynamicArray<GRAPHICS::TIndexSize> indices;
		DynamicArray<GRAPHICS::Mesh3D> meshes;

		HashMap<UIndex32, UIndex32> meshIdToMaterialId;

		float globalScale = 1.0f;
		glm::mat4 modelTransform = glm::mat4(1.0f);

		tinygltf::Model gltfModel{};

		GltfModelInfo modelInfo{};

	};
}
