#pragma once

#include "ApiCall.h"
#include "DefineAs.h"

#include "OwnedPtr.h"
#include "GpuMemoryUsageInfo.h"

#include <optional>

namespace OSK::GRAPHICS {

	class ICommandPool;
	
	/// @brief Representa una conexi�n con la GPU.
	class OSKAPI_CALL IGpu {

	public:

		virtual ~IGpu() = default;

		OSK_DEFINE_AS(IGpu)

		/// @brief Crea una pool de comandos, para poder crear listas de comandos gr�ficos y de presentaci�n.
		/// @return Command pool para comandos gr�ficos y de presentaci�n.
		/// @throws CommandPoolCreationException Si ocurre alg�n error. 
		virtual OwnedPtr<class ICommandPool> CreateGraphicsCommandPool() = 0;

		/// @brief Crea una pool de comandos, para poder crear listas de comandos de computaci�n.
		/// @return Command pool para comandos de computaci�n.
		/// @throws CommandPoolCreationException Si ocurre alg�n error. 
		virtual OwnedPtr<ICommandPool> CreateComputeCommandPool() = 0;


		/// @brief Intenta crear un pool de comandos que soporte �nicamente operaciones de transferencia.
		/// @return Command pool para comandos excusivamente de transferencia.
		/// Si no hay, retorna vac�o.
		virtual std::optional<OwnedPtr<ICommandPool>> CreateTransferOnlyCommandPool() = 0;


		/// @brief Obtiene la informaci�n sobre el uso de memoria de esta GPU,
		/// incluyendo espacio usado y espacio disponible.
		/// @return Informaci�n sobre el uso de memoria de esta GPU.
		virtual GpuMemoryUsageInfo GetMemoryUsageInfo() const = 0;


		/// @brief Cierra la conexi�n con la GPU.
		virtual void Close() = 0;

		/// @return Nombre de la GPU.
		std::string_view GetName() const;

	protected:

		void _SetName(const std::string& name);

	private:

		std::string m_name;

	};

}
