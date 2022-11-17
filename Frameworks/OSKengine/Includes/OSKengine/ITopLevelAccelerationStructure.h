#pragma once

#include "DynamicArray.hpp"
#include "IBottomLevelAccelerationStructure.h"

namespace OSK::GRAPHICS {

	/// <summary>
	/// Estructura de aceleraci�n de nivel alto.
	/// Contiene la geometr�a a renderizar mediante trazado de rayos, la cual 
	/// estar� contenida en las estructuras de nivel bajo IBottomLevelAccelerationStructure.
	/// </summary>
	/// 
	/// @todo Implementaci�n en DirectX 12.
	class OSKAPI_CALL ITopLevelAccelerationStructure {

	public:

		OSK_DEFINE_AS(ITopLevelAccelerationStructure);

		/// <summary>
		/// A�ade una estructura de nivel bajo a la estructura de nivel alto.
		/// </summary>
		/// 
		/// @note A�adirla despu�s de llamar a ITopLevelAccelerationStructure::Setup no har� nada.
		/// @note No se a�adir� propiamente a la estructura de nivel superior hasta que se llame a 
		/// ITopLevelAccelerationStructure::Setup.
		void AddBottomLevelAccelerationStructure(IBottomLevelAccelerationStructure* blas);

		/// <summary>
		/// Crea la estructura de aceleraci�n.
		/// </summary>
		/// 
		/// @pre Debe haber al menos una estructura IBottomLevelAccelerationStructure registrada (se debe haber
		/// llamado a IBottomLevelAccelerationStructure::AddBottomLevelAccelerationStructure al menos una vez).
		/// @pre Todas las estructuras de aceleraci�n de nivel inferior deben haber sido correctamente creadas.
		/// 
		/// @todo Implementaci�n en DX12.
		virtual void Setup() = 0;

		/// <summary>
		/// Actualiza la estructura de aceleraci�n, para que refleje los cambios producidos en
		/// las estructuras de nivel inferior.
		/// </summary>
		/// 
		/// @pre Esta estructura debe haber sido correctamente creada llamando a ITopLevelAccelerationStructure::Setup.
		/// 
		/// @todo Implementaci�n en DX12.
		virtual void Update(ICommandList* cmdList) = 0;

	protected:

		DynamicArray<IBottomLevelAccelerationStructure*> blass;
		bool needsRebuild = false;

	};
	
}
