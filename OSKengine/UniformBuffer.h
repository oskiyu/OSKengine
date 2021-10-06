#pragma once

#include "OSKmacros.h"
#include "GpuDataBuffer.h"
#include "SharedPtr.hpp"

#include <vector>

namespace OSK {

	/// <summary>
	/// Representa un buffer en la GPU que puede ser accedido por slots de materiales.
	/// </summary>
	class OSKAPI_CALL UniformBuffer {

	public:

		/// <summary>
		/// Devuelve los GPU buffers.
		/// </summary>
		const std::vector<SharedPtr<GpuDataBuffer>>& GetBuffers() const;

		/// <summary>
		/// Devuelve los GPU buffers.
		/// </summary>
		std::vector<SharedPtr<GpuDataBuffer>>& GetBuffersRef();

		/// <summary>
		/// Devuelve el tamaño del buffer.
		/// </summary>
		size_t GetBufferSize() const;

		/// <summary>
		/// Devuelve el alignment del buffer.
		/// </summary>
		uint32_t GetBufferAlignment() const;

	private:

		std::vector<SharedPtr<GpuDataBuffer>> buffers;

	};

}
