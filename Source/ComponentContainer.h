#pragma once

#include "IComponentContainer.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "Component.h"
#include "Memory.h"

#include "Assert.h"
#include "InvalidArgumentException.h"

#include <json.hpp>
#include "Serializer.h"

#include "GameObjectManager.h"

#include "SerializableComponent.h"

namespace OSK::ECS {
		
	/// @brief Contenedor para el dynamic array de componentes del tipo dado.
	/// Se encarga de manejar la introducción y eliminación de componentes.
	/// Los componentes se almacenan en memoria contínua.
	/// @tparam TComponent Tipo del componente.
	/// 
	/// @pre TComponent debe validar IsEcsComponent.
	template <typename TComponent> requires IsEcsComponent<TComponent>
	class ComponentContainer : public IComponentContainer {

	public:

		~ComponentContainer() override = default;

		/// @brief Añade un componente asignado al objeto dado.
		/// @param obj Objeto dueño del componente.
		/// @param component Componente a añadir.
		/// @return Referencia no estable al componente añadido.
		/// 
		/// @pre El objeto @p obj no contiene previamente un componente del tipo.
		/// @throws InvalidArgumentException si se incumple la precondición.
		TComponent& AddComponent(GameObjectIndex obj, const TComponent& component) {
			OSK_ASSERT(
				!ObjectHasComponent(obj),
				InvalidArgumentException(std::format("El objeto {} ya contiene el componente.", obj.Get())))

			ComponentIndex componentId = m_components.GetSize();

			m_objectToComponent[obj] = componentId;
			m_componentToObject[componentId] = obj;
			m_components.Insert(component);

			return GetComponent(obj);
		}

		/// @brief Añade un componente asignado al objeto dado.
		/// @param obj Objeto dueño del componente.
		/// @param component Componente a añadir.
		/// @return Referencia no estable al componente añadido.
		/// 
		/// @pre El objeto @p obj no contiene previamente un componente del tipo.
		/// @throws InvalidArgumentException si se incumple la precondición.
		TComponent& AddComponentMove(GameObjectIndex obj, TComponent&& component) {
			OSK_ASSERT(
				!ObjectHasComponent(obj),
				InvalidArgumentException(std::format("El objeto {} ya contiene el componente.", obj.Get())))

			const ComponentIndex componentId = m_components.GetSize();

			m_objectToComponent[obj] = componentId;
			m_componentToObject[componentId] = obj;
			m_components.Insert(std::move(component));

			return GetComponent(obj);
		}
				
		/// @brief Elimina el componente cuyo dueño es el dado.
		/// @param obj Dueño del componente.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		/// @throws InvalidArgumentException Si el objeto no contiene el componente dado.
		void RemoveComponent(GameObjectIndex obj) {
			OSK_ASSERT(
				m_objectToComponent.contains(obj), 
				InvalidArgumentException(std::format("El objeto {} no contiene el componente.", obj.Get())))

			ComponentIndex compIndex = m_objectToComponent.at(obj);
			const UIndex64 indexOfLast = m_components.GetSize() - 1;

			// Hacemos que el componente a eliminar esté en la última posición.
			// El último componente se coloca en el hueco del componente eliminado,
			// para evitar dejar huecos en el array.
			std::swap(m_components[compIndex], m_components[indexOfLast]);

			// Obtenemos el objeto dueño del componente que acabamos de colocar
			// donde el componente eliminado.
			// Actualizamos las tablas para que apunten al nuevo lugar que ocupa.
			GameObjectIndex objectOfLast = m_componentToObject.at(indexOfLast);
			m_objectToComponent.at(objectOfLast) = compIndex;
			m_componentToObject.at(compIndex) = objectOfLast;

			// Eliminación final.
			m_components.RemoveLast();

			m_objectToComponent.erase(obj);
			m_componentToObject.erase(indexOfLast);
		}


		/// @param obj ID del objeto.
		/// @return Referencia no estable al componente del objeto dado.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		TComponent& GetComponent(GameObjectIndex obj) {
			return m_components.At(m_objectToComponent.at(obj));
		}

		/// @param obj ID del objeto.
		/// @return Referencia no estable al componente del objeto dado.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		const TComponent& GetComponent(GameObjectIndex obj) const {
			return m_components.At(m_objectToComponent.at(obj));
		}

		void* GetComponentAddress(GameObjectIndex obj) override {
			auto iterator = m_objectToComponent.find(obj);

			if (iterator == m_objectToComponent.end()) {
				return nullptr;
			}

			return std::addressof(m_components[iterator->second]);
		}

		const void* GetComponentAddress(GameObjectIndex obj) const override {
			const auto iterator = m_objectToComponent.find(obj);

			if (iterator == m_objectToComponent.cend()) {
				return nullptr;
			}

			return std::addressof(m_components[iterator->second]);
		}

		/// @brief Elimina el componente del objeto eliminado.
		/// @param obj ID del objeto.
		/// 
		/// @warning No comprueba que el objeto tenga el componente.
		/// @pre El objeto debe tener el componente.
		void GameObjectDestroyerd(GameObjectIndex obj) override {
			if (ObjectHasComponent(obj)) {
				RemoveComponent(obj);
			}
		}

		/// @param obj Objeto a comprobar.
		/// @return True si tiene el componente añadido.
		bool ObjectHasComponent(GameObjectIndex obj) const {
			return m_objectToComponent.contains(obj);
		}

		/// @return Tipo de componente que almacena este contenedor.
		std::string_view GetComponentTypeName() const override {
			return TComponent::GetComponentTypeName();
		}


		/// @brief Serializa todos los componentes en una estructura
		/// JSON. Contiene:
		/// - "component_type"
		/// - "components": array de JSONs con los datos de los componentes.
		/// - "objects_to_components": enlaza un objeto con un componente.
		/// - "components_to_objects": enlaza un objeto con un componente.
		/// @return JSON serializado.
		nlohmann::json SerializeAll() const override {
			auto output = nlohmann::json();

			if constexpr (IsSerializableComponent<TComponent>) {

				output["component_type"] = TComponent::GetComponentTypeName();

				for (UIndex64 componentIdx = 0; componentIdx < m_components.GetSize(); componentIdx++) {
					const auto& objIndex = m_componentToObject.at(componentIdx);
					const auto& component = m_components[componentIdx];

					output["components"][std::to_string(objIndex.Get())] = PERSISTENCE::SerializeComponent<TComponent>(component);
				}

				for (const auto& [obj, comp] : m_objectToComponent) {
					output["objects_to_components"][std::to_string(obj.Get())] = comp;
				}

				for (const auto& [obj, comp] : m_objectToComponent) {
					output["components_to_objects"][std::to_string(comp)] = obj.Get();
				}

			}

			return output;
		}

		/// @brief Serializa todos los componentes en una estructura
		/// binaria. Contiene:
		/// - Número de componentes (USize64).
		/// - Offset al array de objetos (USize64).
		/// - Datos binarios de todos los componentes.
		/// - Mapa: posición = ID del componente, valor = ID de objeto.
		/// @return Bloque con los datos de los componentes.
		PERSISTENCE::BinaryBlock BinarySerializeAll() const override {
			PERSISTENCE::BinaryBlock output{};

			if constexpr (IsSerializableComponent<TComponent>) {

				output.Write<USize64>(m_components.GetSize());

				// Datos.
				auto datasBlock = PERSISTENCE::BinaryBlock::Empty();
				for (const auto& component : m_components) {
					datasBlock.AppendBlock(PERSISTENCE::BinarySerializeComponent<TComponent>(component));
				}

				// Offset del bloque de objetos enlazados.
				// = tamaño de datos + lo escrito hasta ahora.
				output.Write<USize64>(datasBlock.GetCurrentSize() + output.GetCurrentSize() + sizeof(USize64));

				output.AppendBlock(datasBlock);

				// Mapa ID componente -> ID objeto.
				for (UIndex64 compIdx = 0; compIdx < m_components.GetSize(); compIdx++) {
					output.Write<GameObjectIndex::TUnderlyingType>(m_componentToObject.at(compIdx).Get());
				}

			}

			return output;
		}


		/// @brief Deserializa un componente a partir de un JSON.
		/// @param data JSON con los datos del componente.
		/// @param obj ID del objeto que va a poseer el componente.
		/// @param translator Traductor de IDs.
		/// 
		/// @pre @p data debe haber sido creado respetando el formato del
		/// tipo de componente.
		/// @post El componente se habrá añadido al contenedor.
		void DeserializeComponent(const nlohmann::json& data, GameObjectIndex obj, const SavedGameObjectTranslator& translator) override {
			if constexpr (IsSerializableComponent<TComponent>) {
				AddComponentMove(obj, PERSISTENCE::DeserializeComponent<TComponent>(data, translator));
			}
		}

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
		void BinaryDeserializeComponent(PERSISTENCE::BinaryBlockReader* reader, GameObjectIndex obj, const SavedGameObjectTranslator& translator) override {
			if constexpr (IsSerializableComponent<TComponent>) {
				AddComponentMove(obj, PERSISTENCE::BinaryDeserializeComponent<TComponent>(reader, translator));
			}
		}

	private:

		/// @brief Array con todos los componentes de un mismo tipo.
		DynamicArray<TComponent> m_components;

		/// @brief Mapa dueño -> componente.
		std::unordered_map<GameObjectIndex, ComponentIndex> m_objectToComponent;
		
		/// @brief Mapa componente -> dueño.
		std::unordered_map<ComponentIndex, GameObjectIndex> m_componentToObject;

	};

}
