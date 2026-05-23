#pragma once

#include "TouchInput.h"
#include "IQueryInterface.h"
#include "ApiCall.h"
#include "DynamicArray.hpp"

namespace OSK::IO {

	/// @brief Interfaz paraa obtener toques en 
	/// una pantalla táctil.
	class OSKAPI_CALL ITouchInput {

	public:

		virtual ~ITouchInput() = default;

		OSK_DEFINE_IUUID((TInterfaceUuid)IUUID::ITouchInput);

		/// @return Todos los toques en pantalla táctil
		/// que han ocurrido en este frame.
		const DynamicArray<TouchInput>& GetCurrentFrameInputs() const;

	protected:

		/// @brief Almacena las entradas en un frame.
		/// Se debe limpiar cada frame.
		DynamicArray<TouchInput> m_currentFrameInputs{};

	};

}
