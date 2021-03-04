#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DescriptorSet.h"

namespace OSK {

	//Instancia de un material.
	class OSKAPI_CALL MaterialInstance {

		friend class MaterialPool;

	public:

		//Libera la instancia del material.
		void Free() {
			SafeDelete(&DSet);
		}

	private:

		DescriptorSet* DSet = nullptr;

	};

}
