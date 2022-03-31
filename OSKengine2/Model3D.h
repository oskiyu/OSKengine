#pragma once

#include "IAsset.h"
#include "DynamicArray.hpp"
#include "OwnedPtr.h"

namespace OSK::GRAPHICS {
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
	class Mesh3D;
}

namespace OSK::ASSETS {

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
		void AddMesh(const GRAPHICS::Mesh3D& mesh);

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

	private:

		OwnedPtr<GRAPHICS::IGpuVertexBuffer> vertexBuffer;
		OwnedPtr<GRAPHICS::IGpuIndexBuffer> indexBuffer;

		DynamicArray<GRAPHICS::Mesh3D> meshes;

		TSize numIndices = 0;

	};

}
