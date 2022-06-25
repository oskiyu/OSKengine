#pragma once

#include "IGpuDataBuffer.h"
#include "VertexInfo.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Un vertex buffer es un buffer de memoria en la GPU que almacena los
	/// vértices de un modelo 3D o un sprite.	
	/// </summary>
	class OSKAPI_CALL IGpuVertexBuffer : public GpuDataBuffer {

	public:

		IGpuVertexBuffer(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment, TSize numVertices, const VertexInfo& vertexInfo);

		virtual ~IGpuVertexBuffer() = default;

		template <typename T> T* As() const requires std::is_base_of_v<IGpuVertexBuffer, T> {
			return (T*)this;
		}

		TSize GetNumVertices() const;
		const VertexInfo& GetVertexInfo() const;

	private:

		TSize numVertices = 0;
		VertexInfo vertexInfo{};

	};

}
