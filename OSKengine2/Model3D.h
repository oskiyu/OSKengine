#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"

namespace OSK {

	class IGpuVertexBuffer;
	class IGpuIndexBuffer;

	/// <summary>
	/// Un modelo 3D, para el renderizado 3D.
	/// </summary>
	class OSKAPI_CALL Model3D : public IAsset {

	public:

		Model3D(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("Model3D");

		void _SetVertexBuffer(const OwnedPtr<IGpuVertexBuffer>& vertexBuffer);
		void _SetIndexBuffer(const OwnedPtr<IGpuIndexBuffer>& indexBuffer);

		void _SetIndexCount(TSize count);

		/// <summary>
		/// Devuelve el buffer de la GPU con los
		/// vértices del modelo.
		/// </summary>
		IGpuVertexBuffer* GetVertexBuffer() const;

		/// <summary>
		/// Devuelve el buffer de la GPU con los
		/// índices del modelo.
		/// </summary>
		IGpuIndexBuffer* GetIndexBuffer() const;

		/// <summary>
		/// Número de vértices / índices.
		/// </summary>
		TSize GetIndexCount() const;

	private:

		OwnedPtr<IGpuVertexBuffer> vertexBuffer;
		OwnedPtr<IGpuIndexBuffer> indexBuffer;

		TSize numIndices = 0;

	};

}
