#pragma once

#include "IAsset.h"
#include "OwnedPtr.h"

namespace OSK::GRAPHICS {
	class IGpuVertexBuffer;
	class IGpuIndexBuffer;
}

namespace OSK::ASSETS {

	/// <summary>
	/// Un modelo 3D, para el renderizado 3D.
	/// </summary>
	class OSKAPI_CALL Model3D : public IAsset {

	public:

		Model3D(const std::string& assetFile);

		OSK_ASSET_TYPE_REG("OSK::Model3D");

		void _SetVertexBuffer(const OwnedPtr<GRAPHICS::IGpuVertexBuffer>& vertexBuffer);
		void _SetIndexBuffer(const OwnedPtr<GRAPHICS::IGpuIndexBuffer>& indexBuffer);

		void _SetIndexCount(TSize count);

		/// <summary>
		/// Devuelve el buffer de la GPU con los
		/// v�rtices del modelo.
		/// </summary>
		GRAPHICS::IGpuVertexBuffer* GetVertexBuffer() const;

		/// <summary>
		/// Devuelve el buffer de la GPU con los
		/// �ndices del modelo.
		/// </summary>
		GRAPHICS::IGpuIndexBuffer* GetIndexBuffer() const;

		/// <summary>
		/// N�mero de v�rtices / �ndices.
		/// </summary>
		TSize GetIndexCount() const;

	private:

		OwnedPtr<GRAPHICS::IGpuVertexBuffer> vertexBuffer;
		OwnedPtr<GRAPHICS::IGpuIndexBuffer> indexBuffer;

		TSize numIndices = 0;

	};

}
