#pragma once

#include "ApiCall.h"
#include "DefineAs.h"
#include "OwnedPtr.h"


namespace OSK::GRAPHICS {

	class ICommandList;
	class IGpu;

	enum class CommandsSupport;
	enum class GpuQueueType;


	/// @brief Una pool de comandos se encarga de crear una serie de listas de comandos.
	/// Al mismo tiempo, al destruir el pool se destruirán cada una de las listas creadas por el pool.
	class OSKAPI_CALL ICommandPool {

	public:

		virtual ~ICommandPool() = default;

		OSK_DEFINE_AS(ICommandPool);


		/// @brief Crea una lista de comandos.
		/// @param device GPU donde se alamcenarán las listas.
		/// @return Lista de comandos.
		/// 
		/// @throws CommandListCreationException si no se pudo crear la lista. 
		virtual OwnedPtr<ICommandList> CreateCommandList(const IGpu& device) = 0;

		/// @brief Crea una lista de comandos de un sólo uso.
		/// @param device GPU donde se alamcenarán las listas.
		/// @return Lista de comandos de un único uso.
		/// 
		/// @throws CommandListCreationException si no se pudo crear la lista. 
		virtual OwnedPtr<ICommandList> CreateSingleTimeCommandList(const IGpu& device) = 0;


		/// @return Tipos de comandos soportados por este pool.
		/// Las listas de comandos creadas a partir de este pool
		/// sólamente soportarán los comandos del tipo indicado.
		CommandsSupport GetSupportedCommands() const;

		/// @return Tipo de cola en la que se puede insertar
		/// las listas creadas por este pool.
		GpuQueueType GetLinkedQueueType() const;

	protected:

		/// @brief Crea la lista.
		/// @param supportedCommands Tipos de comandos soportados.
		/// @param mainType Tipo de cola en la que se puede insertar
		/// las listas creadas por este pool.
		ICommandPool(
			CommandsSupport supportedCommands,
			GpuQueueType mainType);

	private:

		CommandsSupport m_supportedCommands;
		GpuQueueType m_mainQueueType;

	};

}
