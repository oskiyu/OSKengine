#pragma once

#include "ICommandList.h"

namespace OSK {

	class OSKAPI_CALL CommandListOgl : public ICommandList {

	public:

		void Reset() override;
		void Start() override;
		void Close() override;

	};

}
