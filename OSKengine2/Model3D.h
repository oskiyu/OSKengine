#pragma once

#include "IAsset.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "IGpuImage.h"
#include "Mesh3D.h"
#include "GpuBuffer.h"

#include "GpuBuffer.h"
#include "IBottomLevelAccelerationStructure.h"

#include "Animator.h"

#include <string>

namespace OSK::GRAPHICS {
	class IMaterialSlot;
}

namespace OSK::ASSETS {

	enum class ModelMobilityType {
		STATIC,
		MOSTLY_STATIC,
		DYNAMIC
	};


	enum class ModelType {
		/// <summary> Malla estática sin animaciones. </summary>
		STATIC_MESH,
		/// <summary> Modelo con animaciones 3D. </summary>
		ANIMATED_MODEL
	};


	/// @brief Contiene datos necesarios para seguir procesando los meshes.
	/// Estos datos se pueden procesar de manera distinta dependiendo del proceso
	/// de renderizado implementado.
	struct MeshMetadata {

		/// @brief Nombre de la textura -> ID de la textura dentro de las texturas almacenadas por el modelo.
		std::unordered_map<std::string, USize32, StringHasher, std::equal_to<>> textureTable;

		/// @brief ID del material dentro de los materiales almacenados por el modelo.
		UIndex64 materialId = 0;

	};


	/// @brief Contiene datos necesarios para seguir procesando el modelo.
	/// Estos datos se pueden procesar de manera distinta dependiendo del proceso
	/// de renderizado implementado.
	struct ModelMetadata {

		/// @brief Índice del mesh -> metadatos del mesh.
		DynamicArray<MeshMetadata> meshesMetadata;

		/// @brief Texturas del modelo 3D.
		DynamicArray<UniquePtr<GRAPHICS::GpuImage>> textures;

		/// @brief Buffers con la información de los materiales.
		DynamicArray<UniquePtr<GRAPHICS::GpuBuffer>> materialInfos;


		constexpr static std::string_view BaseColorTextureName = "baseColorTexture";
		constexpr static std::string_view NormalTextureName = "normalTexture";

	};


	/// @brief Un modelo 3D, para el renderizado 3D.
	/// Está compuesto por una serie de meshes 3D.
	class OSKAPI_CALL Model3D : public IAsset {

	public:

		/// @brief Indica los meshes pertenecientes al modelo
		/// en un nivel de detalle en concreto.
		struct Lod {

			/// @brief Nivel LOD más alto posible.
			constexpr static UIndex64 HighestLevel = 0;

			/// @brief Índice del primer mesh del LOD.
			UIndex64 firstMeshId = 0;

			/// @brief Número de meshes del LOD.
			USize64  meshesCount = 0;

		};

	public:

		explicit Model3D(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::Model3D");


		/// @param level Nivel de LOD deseado.
		/// Más alto = menos detalle.
		/// @return LOD del nivel dado.
		/// Si @p level es mayor que el número de niveles
		/// disponibles, se devuelve el LOD con el nivel
		/// más cercano.
		const Lod& GetLod(UIndex64 level);

		/// @return Nivel de LOD más detallado (número más bajo).
		UIndex64 GetHighestLod() const;

		/// @return Nivel de LOD menos detallado (número más alto).
		UIndex64 GetLowestLod() const;

		/// @brief Registra un LOD.
		/// @param lod Lod a añadir.
		void _RegisterLod(const Lod& lod);


		void _SetId(UIndex64 id);

		/// @return ID único del modelo 3D.
		UIndex64 GetId() const;


		/// @return Devuelve todos los meshes del modelo.
		const DynamicArray<GRAPHICS::Mesh3D>& GetMeshes() const;

		/// @return Devuelve todos los meshes del modelo.
		DynamicArray<GRAPHICS::Mesh3D>& GetMeshes();


		/// @brief Añade un mesh al modelo.
		/// @param mesh Mesh a añadir.
		/// @param metadata Metadatos del mesh.
		void AddMesh(const GRAPHICS::Mesh3D& mesh, const MeshMetadata& metadata);


		/// @brief Establece el tipo de renderepass para el que se perparó
		/// el modelo.
		/// @param renderPassType Tipo de renderpass.
		void SetRenderPassType(const std::string& renderPassType);

		/// @return Renderpass para el que el modelo fue preparado.
		std::string_view GetRenderPassType() const;


		/// @brief Establece el buffer con todos los vértices del modelo 3D.
		/// @param vertexBuffer Buffer con todos los vértices de los meshes del modelo.
		void _SetVertexBuffer(const OwnedPtr<GRAPHICS::GpuBuffer>& vertexBuffer);

		/// @brief Establece el buffer con todos los índices del modelo 3D.
		/// @param indexBuffer Buffer con todos los índices de los meshes del modelo.
		void _SetIndexBuffer(const OwnedPtr<GRAPHICS::GpuBuffer>& indexBuffer);


		/// <summary> Establece el número de índices totales del modelo. </summary>
		/// @pre Debe ser múltiplo de 3.
		/// @pre Debe ser mayor que 0.
		void _SetIndexCount(USize32 count);


		/// <summary> Devuelve el buffer de la GPU con los vértices del modelo. </summary>
		/// 
		/// @note No es null.
		GRAPHICS::GpuBuffer* GetVertexBuffer() { return vertexBuffer.GetPointer(); }
		const GRAPHICS::GpuBuffer* GetVertexBuffer() const { return vertexBuffer.GetPointer(); }

		/// <summary> Devuelve el buffer de la GPU con los índices del modelo. </summary>
		/// 
		/// @note No puede ser null.
		GRAPHICS::GpuBuffer* GetIndexBuffer() { return indexBuffer.GetPointer(); }
		const GRAPHICS::GpuBuffer* GetIndexBuffer() const { return indexBuffer.GetPointer(); }


		/// @brief Número de índices.
		/// 
		/// @note Múltiplo de 3.
		/// @note Mayor que 0.
		USize32 GetIndexCount() const;

		/// <summary> Añade una imagen a la lista de imágenes de los meshes. </summary>
		/// 
		/// @note El índice de esta imagen será su posición dentro de la lista.
		void AddGpuImage(OwnedPtr<GRAPHICS::GpuImage> image);

		void _AddMaterialBuffer(OwnedPtr<GRAPHICS::GpuBuffer> buffer);

		/// <summary> Devuelve la imagen de mesh con el índice dado. </summary>
		/// <param name="index">Índice de la imagen.</param>
		/// @warning No comprueba que esté dentro de los límites.
		const GRAPHICS::GpuImage* GetImage(UIndex32 index) const;

		/// <summary> Devuelve los metadatos para proseguir el procesado del modelo. </summary>
		const ModelMetadata& GetMetadata() const;


		/// <summary>
		/// Establece el animator que contendrá y manejará
		/// las animaciones del modelo 3D.
		/// </summary>
		///
		/// @pre Debe ser un modelo animado.
		void _SetAnimator(GRAPHICS::Animator&& animator);

		/// <summary> Devuelve el animator del modelo 3D. </summary>
		/// 
		/// @pre Debe ser un modelo animado.
		/// @warning Será nullptr si no se cumple la precondición.
		GRAPHICS::Animator* GetAnimator() { return m_animator.GetPointer(); }
		const GRAPHICS::Animator* GetAnimator() const { return m_animator.GetPointer(); }


		ModelType GetType() const;

	private:

		/// @brief ID del modelo 3D.
		UIndex64 m_modelId = 0;

		DynamicArray<Lod> m_lods;

		/// @brief Tipo de pase de renderizado.
		std::string m_renderPassType = "";


		/// @brief Buffer con los vértices de todos los meshes del modelo.
		UniquePtr<GRAPHICS::GpuBuffer> vertexBuffer;

		/// @brief Buffer con los índice de todos los meshes del modelo.
		UniquePtr<GRAPHICS::GpuBuffer> indexBuffer;


		/// @pre Debe ser un modelo animado.
		UniquePtr<GRAPHICS::Animator> m_animator;

		DynamicArray<GRAPHICS::Mesh3D> m_meshes;

		ModelMetadata m_metadata;

		USize32 m_numIndices = 0;

	};

}
