#pragma once

#include "DynamicArray.hpp"
#include "IBottomLevelAccelerationStructure.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Estructura de aceleración de nivel alto.
	/// Contiene la geometría a renderizar mediante trazado de rayos, la cual 
	/// estará contenida en las estructuras de nivel bajo IBottomLevelAccelerationStructure.
	/// </summary>
	/// 
	/// @todo Implementación en DirectX 12.
	class OSKAPI_CALL ITopLevelAccelerationStructure {

	public:

		OSK_DEFINE_AS(ITopLevelAccelerationStructure);

		/// <summary>
		/// Añade una estructura de nivel bajo a la estructura de nivel alto.
		/// </summary>
		/// 
		/// @note Añadirla después de llamar a ITopLevelAccelerationStructure::Setup no hará nada.
		/// @note No se añadirá propiamente a la estructura de nivel superior hasta que se llame a 
		/// ITopLevelAccelerationStructure::Setup.
		void AddBottomLevelAccelerationStructure(IBottomLevelAccelerationStructure* blas);

		/// <summary>
		/// Crea la estructura de aceleración.
		/// </summary>
		/// 
		/// @pre Debe haber al menos una estructura IBottomLevelAccelerationStructure registrada (se debe haber
		/// llamado a IBottomLevelAccelerationStructure::AddBottomLevelAccelerationStructure al menos una vez).
		/// @pre Todas las estructuras de aceleración de nivel inferior deben haber sido correctamente creadas.
		/// 
		/// @todo Implementación en DX12.
		virtual void Setup() = 0;

		/// <summary>
		/// Actualiza la estructura de aceleración, para que refleje los cambios producidos en
		/// las estructuras de nivel inferior.
		/// </summary>
		/// 
		/// @pre Esta estructura debe haber sido correctamente creada llamando a ITopLevelAccelerationStructure::Setup.
		/// 
		/// @todo Implementación en DX12.
		virtual void Update(ICommandList* cmdList) = 0;

	protected:

		DynamicArray<IBottomLevelAccelerationStructure*> blass;
		bool needsRebuild = false;

	};
	
}
