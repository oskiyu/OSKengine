#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>
#include <array>

#ifdef OSK_VULKAN
#include <vulkan/vulkan.h>
#endif // OSK_VULKAN


namespace OSK {


	namespace __VERTEX {

		//Máximo número de 'bones' que un vértice puede seguir.
		OSK_INFO_DO_NOT_TOUCH
		constexpr int32_t MAX_BONES_AMOUNT = 12;

	}

	//Un vértice 3D.
	struct OSKAPI_CALL OldVertex {

		//Posición.
		Vector3 Position;

		//Vector normal.
		Vector3 Normal;

		//Textura.
		Vector2 TexCoords;

		//'Bones' que este vértice sigue.
		float_t BoneID[OSK::__VERTEX::MAX_BONES_AMOUNT] = { 0.0f };

		//'Peso' de los 'bones' que este vértice sigue.
		float_t BoneWeights[OSK::__VERTEX::MAX_BONES_AMOUNT] = { 0.0f };

		uint32_t ID = 0;

		void AddBoneData(const uint32_t& boneID, const float_t& boneWeight);

#ifdef OSK_VULKAN
		static VkVertexInputBindingDescription GetBindingDescription();

		static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
#endif 

	};

}