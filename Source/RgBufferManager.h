#pragma once

#include "ApiCall.h"
#include "DynamicArray.hpp"
#include "UniquePtr.hpp"

#include "GpuBuffer.h"
#include "ImageUuid.h"
#include "RenderGraphArgs.h"
#include "RgEntry.h"

#include <span>
#include <unordered_map>

namespace OSK::GRAPHICS {

	class IRenderer;
	class IRenderPass;


	/// @brief Implementa el manejo de buffers
	/// en el render-graph.
	/// 
	/// @note Es dueño de los buffers.
	class OSKAPI_CALL RgBufferManager {

	public:

		explicit RgBufferManager(IRenderer* renderer);

		/// @brief Registra un nuevo buffer, que será
		/// creado y gestionado por el render-graph.
		/// @param info Información del buffer.
		/// 
		/// @return UUID del buffer.
		/// 
		/// @note No crea el buffer, solo lo registra.
		GdrBufferUuid RegisterBuffer(const RgBufferRegisterInfo& info);

		/// @brief Registra un nuevo buffer, que será
		/// creado y gestionado por el render-graph.
		/// @param info Información del buffer.
		/// @param reservedUuid UUID reservado donde se registrará el buffer.
		/// 
		/// @return UUID del buffer.
		/// 
		/// @note No crea el buffer, solo lo registra.
		GdrBufferUuid RegisterBuffer(
			const RgBufferRegisterInfo& info,
			GdrBufferUuid reservedUuid);

		/// @brief Reserva un UUID que será creado más adelante.
		/// @return UUID reservado pero no registrado.
		GdrBufferUuid ReserveUuid();

		/// @brief Añade una dependencia de uso a un buffer.
		/// @param uuid Buffer cuya dependencia se quiere añadir.
		/// @param usage Nueva dependencia de uso.
		void AddBufferUse(GdrBufferUuid uuid, RgBufferUsage usage);


		/// @brief Crea los buffers, teniendo en cuenta tanto las
		/// dependencias de los renderpasses como las dependencias
		/// delcaradas mediante la función `AddBufferUse`.
		/// @param renderpasses Renderpasses del render-graph.
		void CreateBuffers(std::span<const RenderPassEntry> renderpasses);

		GpuBuffer& GetBuffer(GdrBufferUuid uuid);
		const GpuBuffer& GetBuffer(GdrBufferUuid uuid) const;

	private:

		std::unordered_map<GdrBufferUuid, DynamicArray<PartialRgBufferDependency>> GetBufferDependencies(std::span<const RenderPassEntry> renderpasses);
		std::unordered_map<GdrBufferUuid, GpuBufferUsage> GetBufferUses(const std::unordered_map<GdrBufferUuid, DynamicArray<PartialRgBufferDependency>>& dependencies);

		IRenderer* m_renderer = nullptr;

		std::unordered_map<GdrBufferUuid, GpuBuffer*> m_buffersMap;
		DynamicArray<UniquePtr<GpuBuffer>> m_buffers;
		std::unordered_map<GdrBufferUuid, RgBufferRegisterInfo> m_bufferInfos;
		std::unordered_map<GdrBufferUuid, DynamicArray<PartialRgBufferDependency>> m_bufferUses{};

	};

}
