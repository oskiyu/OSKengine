#pragma once

#include "Component.h"
#include "Serializer.h"

namespace OSK::ECS {

	/// @brief Comprueba si un componente puede ser serializado
	/// y deserializado.
	/// @tparam TComponent tipo de componente.
	template <typename TComponent>
	concept IsSerializableComponent = IsEcsComponent<TComponent> && requires {
		::OSK::PERSISTENCE::SerializeComponent<TComponent>();
		::OSK::PERSISTENCE::DeserializeComponent<TComponent>();
		::OSK::PERSISTENCE::BinarySerializeComponent<TComponent>();
		::OSK::PERSISTENCE::BinaryDeserializeComponent<TComponent>();
	};

}
