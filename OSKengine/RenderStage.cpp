#include "RenderStage.h"

using namespace OSK;

RenderStage::RenderStage() {

}

RenderStage::~RenderStage() {
	Clear();
}

void RenderStage::SetScene(RenderizableScene* scene) {
	this->scene = scene;
}

void RenderStage::SetRenderTarget(RenderTarget* rtarget, bool transferOwnership) {
	isOwnerOfRTarget = transferOwnership;

	renderTarget = rtarget;
}

void RenderStage::AddSpriteBatch(SpriteBatch* sprtBatch) {
	spriteBatches.push_back(sprtBatch);
}

void RenderStage::RemoveSpriteBatch(SpriteBatch* sprtBatch) {
	spriteBatches.remove(sprtBatch);
}

void RenderStage::RemoveAllSpriteBatches() {
	spriteBatches.clear();
}

void RenderStage::Clear() {
	if (isOwnerOfRTarget)
		delete renderTarget;
}