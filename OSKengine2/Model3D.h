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

#include <string>

namespace OSK::GRAPHICS {
	class IMaterialSlot;
}

namespace OSK::ASSETS {

	/// <summary>
	/// Contiene datos necesarios para seguir procesando los meshes.
	/// Estos datos se pueden procesar de manera distinta dependiendo del proceso
	/// de renderizado implementado.
	/// </summary>
	struct MeshMetadata {

		/// <summary>
		/// Contiene las texturas que deben bindearse a la instancia de material
		/// de cada mesh.
		/// 
		/// @note Nombre de la textura -> ID de la textura dentro de las texturas almacenadas por el modelo.
		/// </summary>
		HashMap<std::string, TSize> materialTextures;

	};

	/// <summary>
	/// Contiene datos necesarios para seguir procesando el modelo.
	/// Estos datos se pueden procesar de manera distinta dependiendo del proceso
	/// de renderizado implementado.
	/// </summary>
	struct ModelMetadata {

		/// <summary>
		/// Metadatos por cada uno de los meshes.
		/// 
		/// @note ID de mesh -> metadatos.
		/// </summary>
		DynamicArray<MeshMetadata> meshesMetadata;

		/// <summary>
		/// Texturas de los meshes.
		/// 
		/// @note Meshes de otros modelos no compartir�n texturas,
		/// aunque usen las mismas.
		/// </summary>
		DynamicArray<UniquePtr<GRAPHICS::GpuImage>> textures;

	};


	/// <summary>
	/// Un modelo 3D, para el renderizado 3D.
	/// Est� compuesto por una serie de meshes 3D.
	/// </summary>
	class OSKAPI_CALL Model3D : public IAsset {

	public:

		Model3D(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		/// <summary>
		/// Devuelve todos los meshes del modelo.
		/// </summary>
		const DynamicArray<GRAPHICS::Mesh3D>& GetMeshes() const;

		/// <summary>
		/// A�ade un mesh al modelo.
		/// </summary>
		void AddMesh(const GRAPHICS::Mesh3D& mesh, const MeshMetadata& metadata);

		/// <summary>
		/// Establece el buffer con todos los v�rtices del modelo.
		/// 
		/// @warning Funci�n interna: no llamar.
		/// </summary>
		void _SetVertexBuffer(const OwnedPtr<GRAPHICS::IGpuVertexBuffer>& vertexBuffer);

		/// <summary>
		/// Establece el buffer con todos los �ndices del modelo.
		/// 
		/// @warning Funci�n interna: no llamar.
		/// </summary>
		void _SetIndexBuffer(const OwnedPtr<GRAPHICS::IGpuIndexBuffer>& indexBuffer);

		/// <summary>
		/// Establece el n�mero de �ndices totales del modelo..
		/// 
		/// @warning Funci�n interna: no llamar.
		/// </summary>
		void _SetIndexCount(TSize count);

		/// <summary>
		/// Devuelve el buffer de la GPU con los v�rtices del modelo.
		/// 
		/// @note No puede ser null.
		/// </summary>
		GRAPHICS::IGpuVertexBuffer* GetVertexBuffer() const;

		/// <summary>
		/// Devuelve el buffer de la GPU con los �ndices del modelo.
		/// 
		/// @note No puede ser null.
		/// </summary>
		GRAPHICS::IGpuIndexBuffer* GetIndexBuffer() const;

		/// <summary>
		/// N�mero de �ndices.
		/// </summary>
		TSize GetIndexCount() const;

		/// <summary>
		/// A�ade una imagen a la lista de im�genes de los meshes.
		/// 
		/// @note El �ndice de esta imagen ser� su posici�n dentro de la lista.
		/// </summary>
		void AddGpuImage(OwnedPtr<GRAPHICS::GpuImage> image);

		/// <summary>
		/// Devuelve la imagen de mesh con el �ndice dado.
		/// 
		/// @warning No comprueba que est� dentro de los l�mites.
		/// </summary>
		/// <param name="index">�ndice de la imagen.</param>
		const GRAPHICS::GpuImage* GetImage(TSize index) const;

		/// <summary>
		/// Devuelve los metadatos para proseguir el procesado del modelo.
		/// </summary>
		const ModelMetadata& GetMetadata() const;

	private:

		UniquePtr<GRAPHICS::IGpuVertexBuffer> vertexBuffer;
		UniquePtr<GRAPHICS::IGpuIndexBuffer> indexBuffer;

		DynamicArray<GRAPHICS::Mesh3D> meshes;

		ModelMetadata metadata;

		TSize numIndices = 0;

	};

}
