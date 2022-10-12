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


	/// <summary>
	/// Información relevante de un material del modelo GLTF.
	/// </summary>
	struct GltfMaterialInfo {

		/// <summary>
		/// Color base.
		/// Para los vértices.
		/// </summary>
		Color baseColor;

		/// <summary>
		/// ID de la textura base.
		/// </summary>
		TSize baseTextureIndex = 0;

		/// <summary>
		/// Factor metálico del material.
		/// Afecta el reflejo metálico.
		/// </summary>
		float metallicFactor = 0.0f;

		/// <summary>
		/// ID de la textura que describe la naturaleza
		/// metálica del material.
		/// </summary>
		TSize metallicTextureIndex = 0;

		/// <summary>
		/// Factor de rugosidad del material.
		/// </summary>
		float roughnessFactor = 0.0f;

		/// <summary>
		/// ID de la textura que describe la rugosidad
		/// del material.
		/// </summary>
		TSize roughnessTextureIndex = 0;

		/// <summary>
		/// True si tiene textura.
		/// </summary>
		bool hasBaseTexture = false;

		/// <summary>
		/// True si tiene textura.
		/// </summary>
		bool hasMetallicTexture = false;

		/// <summary>
		/// True si tiene textura.
		/// </summary>
		bool hasRoughnessTexture = false;

	};

	/// <summary>
	/// Información relevante de un modelo GLTF,
	/// necesario para la correcta generación de meshes.
	/// </summary>
	struct GltfModelInfo {

		/// <summary>
		/// Información sobre todos los materiales del modelo.
		/// ID del material = posición dentro del array.
		/// </summary>
		DynamicArray<GltfMaterialInfo> materialInfos;

	};


	/// <summary>
	/// Clase base para poder importar un modelo GLTF.
	/// Se debe crear una clase hija por cada tipo de vértice
	/// que se quiera cargar.
	/// 
	/// Por defecto hay 2: 
	/// - StaticMeshLoader para modelos 3D no animados (Vertex3D).
	/// - AnimMeshLoader para modelos 3D animados (AnimVertex3D).
	/// </summary>
	class IMeshLoader {

	public:

		/// <summary> Carga el modelo 3D. </summary>
		/// <param name="rawAssetPath">Dirección del archivo .glb.</param>
		/// <param name="globalScale">Escala aplicada al modelo.</param>
		void Load(const std::string& rawAssetPath, const glm::mat4& modelTransform);

		/// <summary> Configura el modelo 3D. </summary>
		/// @note Las clases derivadas deben sobreescribir esta función,
		/// creando dentro el vertex buffer del modelo 3D.
		virtual void SetupModel(Model3D* model);

	protected:

		/// <summary> Devuelve una matriz en formato GLM con el transform del nodo. </summary>
		static glm::mat4 GetNodeMatrix(const tinygltf::Node& node);

		/// <summary> Procesa un nodo. </summary>
		/// @note Debe ser sobreescrito, para poder manejar los tipos de vértices distintos.
		virtual void ProcessNode(const tinygltf::Node& node, TSize nodeId, TSize parentId, const glm::mat4& prevMat) = 0;

		/// <summary> Establece las normales del vértice (si tiene). </summary>
		/// @note Debe ser sobreescrito, para poder manejar los tipos de vértices distintos.
		virtual void SmoothNormals() = 0;

		/// <summary> Carga todas las texturas usadas en el modelo 3D. </summary>
		DynamicArray<OwnedPtr<GRAPHICS::GpuImage>> LoadImages();

		/// <summary> Carga todos los materiales usados en el modelo 3D. </summary>
		DynamicArray<GltfMaterialInfo> LoadMaterials();

		DynamicArray<GRAPHICS::TIndexSize> indices;
		DynamicArray<GRAPHICS::Mesh3D> meshes;

		HashMap<TSize, TSize> meshIdToMaterialId;

		float globalScale = 1.0f;
		glm::mat4 modelTransform = glm::mat4(1.0f);

		tinygltf::Model gltfModel{};

		GltfModelInfo modelInfo{};

	};
}
