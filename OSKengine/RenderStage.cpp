#include "RenderStage.h"

using namespace OSK;

RenderStage::RenderStage() {

}

RenderStage::~RenderStage() {
	Clear();
}

void RenderStage::SetScene(RenderizableScene* scene) {
	Scene = scene;
}

void RenderStage::SetRenderTarget(RenderTarget* rtarget, bool transferOwnership) {
	isOwnerOfRTarget = transferOwnership;

	RTarget = rtarget;
}

void RenderStage::AddSpriteBatch(SpriteBatch* sprtBatch) {
	SpriteBatches.push_back(sprtBatch);
}

void RenderStage::RemoveSpriteBatch(SpriteBatch* sprtBatch) {
	SpriteBatches.remove(sprtBatch);
}

void RenderStage::RemoveAllSpriteBatches() {
	SpriteBatches.clear();
}

void RenderStage::Clear() {
	if (isOwnerOfRTarget)
		delete RTarget;
}