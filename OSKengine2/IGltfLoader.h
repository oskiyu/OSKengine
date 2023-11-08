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


		/// @brief Color base (si no tiene textura de color).
		Color baseColor;

		/// @brief ID de la textura de color.
		UIndex32 colorTextureIndex = 0;


		/// @brief Factor met�lico del material.
		///Entre 0.0 y 1.0.
		float metallicFactor = 0.0f;

		/// @brief ID de la textura que describe la naturaleza
		/// met�lica del material.
		UIndex32 metallicTextureIndex = 0;


		/// @brief Factor de rugosidad del material.
		/// Entre 0 y 1.
		float roughnessFactor = 0.0f;

		/// @brief ID de la textura que describe la rugosidad
		/// del material.
		UIndex32 roughnessTextureIndex = 0;


		/// @brief ID de la textura de normales.
		UIndex32 normalTextureIndex = 0;


		/// @brief True si tiene textura.
		bool hasColorTexture = false;

		/// @brief True si tiene textura.
		bool hasMetallicTexture = false;

		/// @brief True si tiene textura.
		bool hasRoughnessTexture = false;

		/// @brief True si tiene textura.
		bool hasNormalTexture = false;

	};


	/// @brief Informaci�n relevante de un modelo GLTF,
	/// necesario para la correcta generaci�n de meshes.
	struct GltfModelInfo {

		/// @brief Informaci�n sobre todos los materiales del modelo.
		/// ID del material = posici�n dentro del array.
		DynamicArray<GltfMaterialInfo> materialInfos;

	};


	/// @brief Clase base para poder importar un modelo GLTF.
	/// 
	/// Incluye m�todos para importar informaci�n relevante del
	/// modelo GLTF.
	class IGltfLoader {

	public:

		virtual ~IGltfLoader() = default;


		/// @brief Carga el modelo 3D.
		/// @param rawAssetPath Direcci�n del archivo "*.glb".
		/// @param modelTransform Transform aplicado al modelo.
		void Load(
			const std::string& rawAssetPath, 
			const glm::mat4& modelTransform);

		/// @brief Carga una escena.
		/// @param scene Escena a cargar.
		virtual void LoadScene(const tinygltf::Scene& scene);

		/// @brief Configura el modelo 3D.
		/// @param model Modelo a configurar.
		/// 
		/// @pre @p model no debe ser null.
		/// 
		/// @note Las clases derivadas deben sobreescribir esta funci�n,
		/// creando dentro el vertex buffer del modelo 3D.
		virtual void SetupModel(Model3D* model);

	protected:

		static DynamicArray<std::string> GetTagsFromName(const tinygltf::Scene& scene);

		/// @brief Devuelve una matriz en formato GLM con el transform del nodo.
		/// @param node Nodo GLTF.
		/// @return Matriz con su transform.
		static glm::mat4 GetNodeMatrix(const tinygltf::Node& node);

		
		/// @brief Procesa un nodo de un modelo GLTF.
		/// Este nodo puede contener o no geometr�a 3D en forma
		/// de meshes.
		/// @param node Nodo GLTF.
		/// @param nodeId ID del nodo procesado.
		/// @param parentId ID del nodo padre (TSize::max si no tiene padre).
		virtual void ProcessNode(const tinygltf::Node& node, UIndex32 nodeId, UIndex32 parentId) = 0;


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

		/// @param primitive Mesh.
		/// @param startOffset �ndice del primer �ndice.
		/// @return Lista con los �ndices de @p primitive.
		DynamicArray<GRAPHICS::TIndexSize> GetIndices(
			const tinygltf::Primitive& primitive, 
			GRAPHICS::TIndexSize startOffset) const;

		/// @param primitive Mesh.
		/// @return Vectores tangentes de los v�rtices del mesh.
		/// @pre El mesh debe tener vectores tangentes preprocesados.
		/// @throws NoVertexTangentsFoundException si no se cumple la precondici�n.
		DynamicArray<Vector3f> GetTangentVectors(const tinygltf::Primitive& primitive) const;

		/// @brief Genera vectores tangentes para los v�rtices indicados.
		/// @param texCoords Coordenadas de texturas UV de los v�rtices.
		/// @param positions Posiciones de los v�rtices.
		/// @param indices �ndices de los v�rtices.
		/// @param indicesStartOffset Primer �ndice de los v�rtices.
		/// @return Vectores tangentes.
		/// 
		/// @pre @p texCoords.GetSize() == @p positions.GetSize()
		/// @pre @p indices.GetSize() debe ser m�ltiplo de 3.
		/// @param indicesStartOffset debe ser m�ltiplo de 3.
		DynamicArray<Vector3f> GenerateTangetVectors(
			const DynamicArray<Vector2f>& texCoords,
			const DynamicArray<Vector3f>& positions,
			const DynamicArray<GRAPHICS::TIndexSize>& indices,
			UIndex32 indicesStartOffset) const;

		/// @param mesh Mesh.
		/// @return Propiedades extra del mesh.
		std::unordered_map<std::string, std::string, StringHasher, std::equal_to<>> GetCustomProperties(const tinygltf::Mesh& mesh) const;


		/// @return Todas las texturas usadas en el modelo 3D.
		DynamicArray<OwnedPtr<GRAPHICS::GpuImage>> LoadImages();

		/// @return Todas los materiales usados en el modelo 3D.
		DynamicArray<GltfMaterialInfo> LoadMaterials();


		DynamicArray<GRAPHICS::TIndexSize> m_indices;
		DynamicArray<GRAPHICS::Mesh3D> m_meshes;

		std::unordered_map<UIndex32, UIndex32> m_meshIdToMaterialId;

		float m_globalScale = 1.0f;
		glm::mat4 m_modelTransform = glm::mat4(1.0f);

		tinygltf::Model m_gltfModel{};

		GltfModelInfo m_modelInfo{};

	};
}
