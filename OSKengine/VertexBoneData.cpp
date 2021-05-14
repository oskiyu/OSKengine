#include "VertexBoneData.h"

using namespace OSK;

void VertexBoneData::Add(uint32_t id, float weight) {
	for (uint32_t i = 0; i < OSK_ANIM_MAX_BONES_PER_VERTEX; i++) {
		if (weights[i] == 0.0f) {
			IDs[i] = id;
			weights[i] = weight;

			return;
		}
	}
}
