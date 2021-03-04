#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

namespace OSK {

	//Tipos de binding del material.
	//
	//	-DATA_BUFFER: buffer de datos: cada instancia tiene su propio buffer. (VK: uniform buffer).
	//	-DYNAMICE_DATA_BUFFER: buffer de datos compartido por cada instancia. (VK: dynamic uniform buffer).
	//	-TEXTURE: textura.
	enum class MaterialBindingType {
		DATA_BUFFER,
		DYNAMIC_DATA_BUFFER,
		TEXTURE
	};

	//Stage del binding: en que shader se accede.
	enum class MaterialBindingShaderStage {
		VERTEX,
		FRAGMENT
	};

	//Información de un binding de un material: tipo y shader al que se enlaza.
	struct OSKAPI_CALL MaterialBinding {
		MaterialBindingType Type;
		MaterialBindingShaderStage Stage;
	};
}