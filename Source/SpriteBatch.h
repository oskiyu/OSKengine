#pragma once

#include "UniquePtr.hpp"

#include "ResourcesInFlight.h"

#include "GpuBuffer.h"
#include "MaterialInstance.h"

#include "Vector2.hpp"
#include "Vector4.hpp"

#include <array>

namespace OSK::GRAPHICS {

	class ICommandList;

	class OSKAPI_CALL SpriteBatch {

	public:

		explicit SpriteBatch(UniquePtr<MaterialInstance>&& materialInstance);
		~SpriteBatch();

		void Draw(
			Vector2f position,
			Vector2f size,
			Vector4f texCoords);

		void ResetCurrentFrame();

		void Draw(ICommandList* commandList);

	public:

		const static USize32 maxVertices = 1024u;
		const static USize32 maxIndices = 1024u;

	private:

		UniquePtr<MaterialInstance> materialInstance;

		std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> vertexBuffers{};
		std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> indexBuffers{};

		USize32 numVertices = 0;
		USize32 numIndices = 0;

	};

}
