#pragma once

#include "ApiCall.h"
#include "DefineAs.h"
#include "OwnedPtr.h"

#include "GpuMemoryUsageInfo.h"
#include "GpuMemoryAlignments.h"

#include <optional>

namespace OSK::GRAPHICS {

	class ICommandPool;
	class IGpuImageSampler;
	struct GpuImageSamplerDesc;

	/// @brief Representa una conexión con la GPU.
	class OSKAPI_CALL IGpu {

	public:

		virtual ~IGpu() = default;

		OSK_DEFINE_AS(IGpu)

		/// @brief Crea una pool de comandos, para poder crear listas de comandos gráficos y de presentación.
		/// @return Command pool para comandos gráficos y de presentación.
		/// @throws CommandPoolCreationException Si ocurre algún error. 
		virtual OwnedPtr<class ICommandPool> CreateGraphicsCommandPool() = 0;

		/// @brief Crea una pool de comandos, para poder crear listas de comandos de computación.
		/// @return Command pool para comandos de computación.
		/// @throws CommandPoolCreationException Si ocurre algún error. 
		virtual OwnedPtr<ICommandPool> CreateComputeCommandPool() = 0;

		/// @brief Crea un sampler con las propiedades indicadas.
		/// @param info Propiedades del sampler.
		/// @return Sampler con las propiedades indicadas.
		virtual OwnedPtr<IGpuImageSampler> CreateSampler(const GpuImageSamplerDesc& info) const = 0;


		/// @brief Intenta crear un pool de comandos que soporte únicamente operaciones de transferencia.
		/// @return Command pool para comandos excusivamente de transferencia.
		/// Si no hay, retorna vacío.
		virtual std::optional<OwnedPtr<ICommandPool>> CreateTransferOnlyCommandPool() = 0;


		/// @brief Obtiene la información sobre el uso de memoria de esta GPU,
		/// incluyendo espacio usado y espacio disponible.
		/// @return Información sobre el uso de memoria de esta GPU.
		virtual GpuMemoryUsageInfo GetMemoryUsageInfo() const = 0;

		/// @return Información sobre el alineamiento que deben
		/// seguir los distintos tipos de recursos.
		const GpuMemoryAlignments& GetMemoryAlignments() const;


		/// @brief Cierra la conexión con la GPU.
		virtual void Close() = 0;

		/// @return Nombre de la GPU.
		std::string_view GetName() const;


		// Capacidades.

		/// @return True si soporta ray-tracing.
		bool SupportsRayTracing() const;

		/// @return True si soporta renderizado bind-less.
		bool SupportsBindlessResources() const;

		/// @return True si soporta renderizado de meshes.
		bool SupportsMeshShaders() const;

	protected:

		void _SetName(const std::string& name);
		void SetMinAlignments(const GpuMemoryAlignments& alignments);

		bool m_supportsRayTracing = false;
		bool m_supportsBindless = false;
		bool m_supportsMesh = false;

	private:

		std::string m_name;
		GpuMemoryAlignments m_minAlignments{};

	};

}
