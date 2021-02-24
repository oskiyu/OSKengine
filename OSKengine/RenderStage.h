#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "RenderTarget.h"
#include "SpriteBatch.h"
#include "RenderizableScene.h"

#include <list>

namespace OSK {

	class OSKAPI_CALL RenderStage {

		friend class RenderAPI;
		friend class RenderSystem3D;

	public:
		
		RenderStage();

		~RenderStage();

		void SetScene(RenderizableScene* scene);
		void SetRenderTarget(RenderTarget* rtarget, bool transferOwnership);

		void AddSpriteBatch(SpriteBatch* sprtBatch);
		void RemoveSpriteBatch(SpriteBatch* sprtBatch);
		void RemoveAllSpriteBatches();

	private:

		void Clear();

		bool isOwnerOfRTarget = false;
		RenderTarget* RTarget = nullptr;

		std::list<SpriteBatch*> SpriteBatches;
		RenderizableScene* Scene = nullptr;

	};

}