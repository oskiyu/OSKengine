#pragma once

#include "IAsset.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "IGpuImage.h"
#include "Mesh3D.h"

#include "IGpuVertexBuffer.h"
#include "IGpuIndexBuffer.h"
#include "IBottomLevelAccelerationStructure.h"

#include "Animator.h"

#include <string>

namespace OSK::GRAPHICS {
	class IMaterialSlot;
}

namespace OSK::ASSETS {

	enum class ModelType {
		/// <summary> Malla estática sin animaciones. </summary>
		STATIC_MESH,
		/// <summary> Modelo con animaciones 3D. </summary>
		ANIMATED_MODEL
	};


	/// <summary>
	/// Contiene datos necesarios para seguir procesando los meshes.
	/// Estos datos se pueden procesar de manera distinta dependiendo del proceso
	/// de renderizado implementado.
	/// </summary>
	struct MeshMetadata {

		/// <summary>
		/// Contiene las texturas que deben bindearse a la instancia de material
		/// de cada mesh.
		/// </summary>
		/// 
		/// @note Nombre de la textura -> ID de la textura dentro de las texturas almacenadas por el modelo.
		HashMap<std::string, TSize> materialTextures;

		/// <summary> Factor metálico del material del mesh. </summary>
		///
		/// @note Entre 0.0 y 1.0, ambos incluidos. 
		float metallicFactor = 0.0f;

		/// <summary> Factor de rugosidad del material del mesh. </summary>
		///
		/// @note Entre 0.0 y 1.0, ambos incluidos. 
		float roughnessFactor = 0.0f;

	};

	/// <summary>
	/// Contiene datos necesarios para seguir procesando el modelo.
	/// Estos datos se pueden procesar de manera distinta dependiendo del proceso
	/// de renderizado implementado.
	/// </summary>
	struct ModelMetadata {

		/// <summary> Metadatos por cada uno de los meshes. </summary>
		/// 
		/// @note Índice del mesh -> metadatos.
		DynamicArray<MeshMetadata> meshesMetadata;

		/// <summary> Texturas de los meshes. </summary>
		/// 
		/// @note Meshes de otros modelos no compartirán texturas,
		/// aunque usen las mismas.
		DynamicArray<UniquePtr<GRAPHICS::GpuImage>> textures;

	};


	/// <summary>
	/// Un modelo 3D, para el renderizado 3D.
	/// Está compuesto por una serie de meshes 3D.
	/// </summary>
	class OSKAPI_CALL Model3D : public IAsset {

	public:

		Model3D(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		/// <summary>  Devuelve todos los meshes del modelo. </summary>
		const DynamicArray<GRAPHICS::Mesh3D>& GetMeshes() const;

		/// <summary> Añade un mesh al modelo. </summary>
		void AddMesh(const GRAPHICS::Mesh3D& mesh, const MeshMetadata& metadata);


		/// <summary> Establece el buffer con todos los vértices del modelo 3D. </summary>
		/// 
		/// @warning Función interna: no llamar.
		void _SetVertexBuffer(const OwnedPtr<GRAPHICS::IGpuVertexBuffer>& vertexBuffer);

		/// <summary> Establece el buffer con todos los índices del modelo. </summary>
		/// 
		/// @warning Función interna: no llamar.
		void _SetIndexBuffer(const OwnedPtr<GRAPHICS::IGpuIndexBuffer>& indexBuffer);

		/// <summary> Establece la estructura de aceleración de nivel bajo. </summary>
		/// 
		/// @pre Debe estar activado el renderizado por trazado de rayos.
		/// @warning Función interna: no llamar.
		void _SetAccelerationStructure(OwnedPtr<GRAPHICS::IBottomLevelAccelerationStructure> accelerationStructure);

		/// <summary> Establece el número de índices totales del modelo. </summary>
		/// 
		/// @pre Debe ser múltiplo de 3.
		/// @pre Debe ser mayor que 0.
		/// 
		/// @warning Función interna: no llamar.
		void _SetIndexCount(TSize count);

		/// <summary> Devuelve el buffer de la GPU con los vértices del modelo. </summary>
		/// 
		/// @note No es null.
		GRAPHICS::IGpuVertexBuffer* GetVertexBuffer() const;

		/// <summary> Devuelve el buffer de la GPU con los índices del modelo. </summary>
		/// 
		/// @note No puede ser null.
		GRAPHICS::IGpuIndexBuffer* GetIndexBuffer() const;

		/// <summary>
		/// Devuelve la estructura de aceleración de nivel bajo del modelo.
		/// Únicamente si se ha cargado con el renderizador en modo de 
		/// trazado de rayos.
		/// </summary>
		/// 
		/// @pre El renderizador debe tener activado el modo de trazado de rayos.
		/// @warning Será null si el renderizador no tiene activo el modo de trazado
		/// de rayos.
		GRAPHICS::IBottomLevelAccelerationStructure* GetAccelerationStructure() const;

		/// <summary> Número de índices.  </summary>
		/// 
		/// @note Múltiplo de 3.
		/// @note Mayor que 0.
		TSize GetIndexCount() const;

		/// <summary> Añade una imagen a la lista de imágenes de los meshes. </summary>
		/// 
		/// @note El índice de esta imagen será su posición dentro de la lista.
		void AddGpuImage(OwnedPtr<GRAPHICS::GpuImage> image);

		/// <summary> Devuelve la imagen de mesh con el índice dado. </summary>
		/// <param name="index">Índice de la imagen.</param>
		/// @warning No comprueba que esté dentro de los límites.
		const GRAPHICS::GpuImage* GetImage(TSize index) const;

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
		GRAPHICS::Animator* GetAnimator() const;


		ModelType GetType() const;

	private:

		UniquePtr<GRAPHICS::IGpuVertexBuffer> vertexBuffer;
		UniquePtr<GRAPHICS::IGpuIndexBuffer> indexBuffer;
		/// @pre El renderizador debe tener el modo de trazado de rayos activado.
		UniquePtr<GRAPHICS::IBottomLevelAccelerationStructure> accelerationStructure;

		/// @pre Debe ser un modelo animado.
		UniquePtr<GRAPHICS::Animator> animator;

		DynamicArray<GRAPHICS::Mesh3D> meshes;

		ModelMetadata metadata;

		TSize numIndices = 0;

	};

}
