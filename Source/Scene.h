#pragma once

#include "OSKmacros.h"
#include "GameObject.h"

#include "Serializer.h"


namespace OSK::ECS {

	class OSKAPI_CALL Scene {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:

	private:

		DynamicArray<ECS::GameObjectIndex> m_objects;

		std::string m_irradianceMapFile = "";
		std::string m_specularMapFile = "";

	};

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::ECS::Scene>(const OSK::ECS::Scene& data);

	template <>
	OSK::ECS::Scene DeserializeJson<OSK::ECS::Scene>(const nlohmann::json& json);

}
