#pragma once

#include "ApiCall.h"
#include "GameObject.h"

// Para ComponentType
#include "Component.h"

// Para serialización.
#include "BinaryBlock.h"
#include <json.hpp>


namespace OSK::ECS {

	class SavedGameObjectTranslator;

	/// @brief Interfaz común para todos los contenedores de componentes.
	class OSKAPI_CALL IComponentContainer {

	public:

		virtual ~IComponentContainer() = default;

		/// @brief Se llama cuando un objeto es destruido.
		/// Se encarga de eliminar todos los componentes del objeto.
		/// @param obj ID del objeto destruido.
		virtual void GameObjectDestroyerd(GameObjectIndex obj) = 0;


		/// @brief Serializa todos los componentes en una estructura
		/// JSON. Contiene:
		/// - "component_type"
		/// - "components": array de JSONs con los datos de los componentes.
		/// - "objects_to_components": enlaza un objeto con un componente.
		/// - "components_to_objects": enlaza un objeto con un componente.
		/// @return JSON serializado.
		virtual nlohmann::json SerializeAll() const = 0;

		/// @brief Deserializa un componente a partir de un JSON.
		/// @param data JSON con los datos del componente.
		/// @param obj ID del objeto que va a poseer el componente.
		/// @param translator Traductor de IDs.
		/// 
		/// @pre @p data debe haber sido creado respetando el formato del
		/// tipo de componente.
		/// @post El componente se habrá añadido al contenedor.
		virtual void DeserializeComponent(
			const nlohmann::json& data,
			GameObjectIndex obj,
			const SavedGameObjectTranslator& translator) = 0;

		/// @brief Serializa todos los componentes en una estructura
		/// binaria. Contiene:
		/// - Número de componentes (USize64).
		/// - Offset al array de objetos (USize64).
		/// - Datos binarios de todos los componentes.
		/// - Mapa: posición = ID del componente, valor = ID de objeto.
		/// @return Bloque con los datos de los componentes.
		virtual PERSISTENCE::BinaryBlock BinarySerializeAll() const = 0;

		/// @brief Deserializa un componente a partir de un bloque binario.
		/// @param reader Lector del bloque binario.
		/// @param obj ID del objeto que va a poseer el componente.
		/// @param translator Traductor de IDs.
		/// 
		/// @pre @p data debe haber sido creado respetando el formato del
		/// tipo de componente.
		/// @post El componente se habrá añadido al contenedor.
		/// @post El cursor de @p reader habrá avanzado, dejando atrás todos
		/// los datos leídos.
		virtual void BinaryDeserializeComponent(
			PERSISTENCE::BinaryBlockReader* reader,
			GameObjectIndex obj,
			const SavedGameObjectTranslator& translator) = 0;


		/// @brief Establece el ID del tipo de componente que maneja el contenedor.
		/// @param componentType Tipo de componente.
		/// @pre @p componentType Debe haber sido previamente registrado.
		void SetComponentType(ComponentType componentType);

		/// @return ID del tipo de componente que maneja el contenedor.
		ComponentType GetComponentType() const;

		/// @return Tipo de componente que almacena este contenedor.
		virtual std::string_view GetComponentTypeName() const = 0;

	private:

		ComponentType m_componentType = 0;

	};

}
