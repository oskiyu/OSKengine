#pragma once

#include "IAsset.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "UniquePtr.hpp"
#include "OwnedPtr.h"
#include "IGpuImage.h"

#include <string>

namespace OSK::GRAPHICS {
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	class Mesh3D;
	class IMaterialSlot;
	class Mesh3D;
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
		/// Nombre de la textura -> ID de la textura dentro de las texturas almacenadas por el modelo.
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
		/// ID de mesh -> metadatos.
		/// </summary>
		DynamicArray<MeshMetadata> meshesMetadata;

		/// <summary>
		/// Texturas de los meshes.
		/// </summary>
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

		/// <summary>
		/// Devuelve todos los meshes del modelo.
		/// </summary>
		const DynamicArray<GRAPHICS::Mesh3D>& GetMeshes() const;

		/// <summary>
		/// Añade un mesh al modelo.
		/// </summary>
		void AddMesh(const GRAPHICS::Mesh3D& mesh, const MeshMetadata& metadata);

		void _SetVertexBuffer(const OwnedPtr<GRAPHICS::IGpuVertexBuffer>& vertexBuffer);
		void _SetIndexBuffer(const OwnedPtr<GRAPHICS::IGpuIndexBuffer>& indexBuffer);

		void _SetIndexCount(TSize count);

		/// <summary>
		/// Devuelve el buffer de la GPU con los
		/// vértices del modelo.
		/// </summary>
		GRAPHICS::IGpuVertexBuffer* GetVertexBuffer() const;

		/// <summary>
		/// Devuelve el buffer de la GPU con los
		/// índices del modelo.
		/// </summary>
		GRAPHICS::IGpuIndexBuffer* GetIndexBuffer() const;

		/// <summary>
		/// Número de vértices / índices.
		/// </summary>
		TSize GetIndexCount() const;

		void AddGpuImage(OwnedPtr<GRAPHICS::GpuImage> image);
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
