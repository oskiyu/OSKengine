#pragma once

#include <cstdint>

namespace OSK {

	/// <summary>
	/// Identifica un tipo de slot.
	/// Puede ampliarse con enums externos.
	/// Para que dos slots sean compatibles, deben tener el mismo layout en el mismo set# en el shader.
	/// </summary>
	enum MaterialSlotType : size_t {
		MSLOT_CAMERA_3D,
		MSLOT_SCENE_3D,
		MSLOT_PER_MODEL_3D,
		MSLOT_PER_INSTANCE_3D,

		MSLOT_TEXTURE_2D,
		MSLOT_CAMERA_2D,

		MSLOT_SKYBOX_CAMERA,
		MSLOT_SKYBOX_TEXTURE,

		MSLOT_SHADOWS_3D_SCENE,
		MSLOT_SHADOWS_3D_CAMERA,
		MSLOT_SHADOWS_3D_BONES,

		MSLOT_END0
	};

	using MaterialSlotTypeId = size_t;

	/// <summary>
	/// Identifica un material.
	/// </summary>
	enum MaterialPipelineType : size_t {
		MPIPE_2D,
		MPIPE_3D,
		MPIPE_SHADOWS3D,
		MPIPE_SKYBOX,
		MPIPE_POSTPROCESS,

		MPIPE_END0
	};

	using MaterialPipelineTypeId = size_t;

}
