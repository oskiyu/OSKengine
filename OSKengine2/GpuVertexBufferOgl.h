#pragma once

#include "IGpuVertexBuffer.h"

#include "OglTypes.h"

namespace OSK::GRAPHICS {

	class OSKAPI_CALL GpuVertexBufferOgl : public IGpuVertexBuffer {

	public:

		GpuVertexBufferOgl(OwnedPtr<IGpuMemorySubblock> buffer, TSize size, TSize alignment);

		/// <summary>
		/// Establece el handler del propio buffer.
		/// </summary>
		void SetBufferHandler(OglVertexBufferHandler handler); 

		/// <summary>
		/// Establece el handler para el array de vértices.
		/// </summary>
		void SetViewHandler(OglVertexBufferHandler handler);

		/// <summary>
		/// Devuelve el handler del propio buffer.
		/// </summary>
		OglVertexBufferHandler GetBufferHandler() const;

		/// <summary>
		/// Devuelve el handler para el array de vértices.
		/// </summary>
		OglVertexBufferHandler GetViewHandler() const;

	private:

		/// <summary>
		/// Handler del propio buffer.
		/// </summary>
		OglVertexBufferHandler bufferHandler = 0;

		/// <summary>
		/// Handler específico para el array de vértices.
		/// </summary>
		OglVertexBufferHandler viewHandler = 0;

	};

}
