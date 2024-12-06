#include "EntityComponentSystem.h"

#include "SystemManager.h"
#include "ComponentManager.h"
#include "GameObjectManager.h"

#include "SavedGameObjectTranslator.h"

#include "FileIO.h"


using namespace OSK;
using namespace OSK::ECS;
using namespace OSK::PERSISTENCE;


EntityComponentSystem::EntityComponentSystem(IO::ILogger* logger) : m_logger(logger) {

}

void EntityComponentSystem::OnTick(TDeltaTime deltaTime) {
	m_systemManager->OnTick(deltaTime, m_eventManager.GetValue());
}

void EntityComponentSystem::OnRender(GRAPHICS::ICommandList* commandList) {
	m_systemManager->OnRender(commandList);
}

void EntityComponentSystem::Save(std::string_view path) const {
	nlohmann::json output{};

	output["file_type"] = "Scene";
	output["spec_ver"] = "0";

	for (const auto& set : m_systemManager->GetExecutionGraph().GetExecutionGraph()) {
		for (const auto& system : set.systems) {
			if (dynamic_cast<const ISerializableSystem*>(system)) {
				output["systems"][static_cast<std::string>(system->GetName())] = dynamic_cast<const ISerializableSystem*>(system)->SaveConfiguration();
			}
		}
	}

	for (const auto& container : m_componentManager->GetAllContainers()) {
		output["components"].push_back(container.second->SerializeAll());
	}

	for (const auto& object : m_gameObjectManager->GetAllLivingObjects()) {
		output["objects"].push_back(object.Get());
	}

	IO::FileIO::WriteFile(static_cast<std::string>(path) + ".json", output.dump(4));
}

void EntityComponentSystem::SaveBinary(std::string_view path) const {
	BinaryBlock data = BinaryBlock::Empty();

	data.Write<TByte>(1); // Scene
	data.Write<TByte>(0); // Spec

	// Sistemas.
	BinaryBlock systemFullBlock = BinaryBlock::Empty();
	{
		// Tabla con los nombres de los sistemas.
		USize64 systemCount = 0;
		BinaryBlock systemNamesBlock = BinaryBlock::Empty();
		{
			for (const auto& set : m_systemManager->GetExecutionGraph().GetExecutionGraph()) {
				for (const auto& system : set.systems) {
					systemCount++;
				}
			}

			for (const auto& set : m_systemManager->GetExecutionGraph().GetExecutionGraph()) {
				for (const auto& system : set.systems) {
					systemNamesBlock.WriteString(system->GetName());
				}
			}
		}

		// Datos de los sistemas.
		BinaryBlock systemDatasBlock = BinaryBlock::Empty();
		{
			for (const auto& set : m_systemManager->GetExecutionGraph().GetExecutionGraph()) {
				for (const auto& system : set.systems) {
					if (dynamic_cast<const ISerializableSystem*>(system)) {
						const auto sSystem = dynamic_cast<const ISerializableSystem*>(system);
						const auto dataBlock = sSystem->SaveBinaryConfiguration();
						const auto blockSize = dataBlock.GetCurrentSize();

						systemDatasBlock.Write<USize64>(blockSize);
						systemDatasBlock.AppendBlock(dataBlock);
					}
				}
			}
		}
		;
		systemFullBlock.Write<USize64>(systemCount);
		systemFullBlock.Write<USize64>(systemNamesBlock.GetCurrentSize() + systemFullBlock.GetCurrentSize() + sizeof(USize64));
		systemFullBlock.AppendBlock(systemNamesBlock);
		systemFullBlock.AppendBlock(systemDatasBlock);
	}

	// Objetos.
	BinaryBlock objectsFullBlock = BinaryBlock::Empty();
	{
		objectsFullBlock.Write<USize64>(m_gameObjectManager->GetAllLivingObjects().size());
		for (const auto& object : m_gameObjectManager->GetAllLivingObjects()) {
			objectsFullBlock.Write<GameObjectIndex::TUnderlyingType>(object.Get());
		}
	}

	// Componentes.
	BinaryBlock componentsFullBlock = BinaryBlock::Empty();
	{
		// Tabla con los nombres de los componentes.
		BinaryBlock componentNamesBlock = BinaryBlock::Empty();
		for (const auto& [name, _] : m_componentManager->GetAllContainers()) {
			componentNamesBlock.WriteString(name);
		}

		// Tabla con los datos de los componentes.
		BinaryBlock componentDatasBlock = BinaryBlock::Empty();
		for (const auto& container : m_componentManager->GetAllContainers()) {
			componentDatasBlock.AppendBlock(container.second->BinarySerializeAll());
		}

		componentsFullBlock.Write<USize64>(m_componentManager->GetAllContainers().size());
		componentsFullBlock.Write<USize64>(componentNamesBlock.GetCurrentSize() + componentsFullBlock.GetCurrentSize() + sizeof(USize64));
		componentsFullBlock.AppendBlock(componentNamesBlock);
		componentsFullBlock.AppendBlock(componentDatasBlock);
	}


	// Número de índices:
	// - Parte de sistemas.
	// - Parte de objetos.
	// - Parte de componentes.
	const USize64 dataStart = 3 * sizeof(UIndex64) + data.GetCurrentSize();

	// Parte de sistemas.
	data.Write<UIndex64>(dataStart);

	// Parte de objetos.
	data.Write<UIndex64>(dataStart + systemFullBlock.GetCurrentSize());

	// Parte de componentes.
	data.Write<UIndex64>(dataStart + systemFullBlock.GetCurrentSize() + objectsFullBlock.GetCurrentSize());

	data.AppendBlock(systemFullBlock);
	data.AppendBlock(objectsFullBlock);
	data.AppendBlock(componentsFullBlock);

	IO::FileIO::WriteBinaryFile(static_cast<std::string>(path) + ".bsf", data.GetData());
}

SavedGameObjectTranslator EntityComponentSystem::LoadScene(std::string_view path) {
	OSK_ASSERT(
		IO::FileIO::FileExists(path),
		InvalidArgumentException(std::format("EL archivo de escena {} no existe.", path)));

	nlohmann::json data = nlohmann::json::parse(IO::FileIO::ReadFromFile(path));

	// Objects
	auto oldObjects = DynamicArray<GameObjectIndex>::CreateReserved(m_gameObjectManager->GetAllLivingObjects().size());

	for (const auto& obj : m_gameObjectManager->GetAllLivingObjects()) {
		oldObjects.Insert(obj);
	}

	for (auto& obj : oldObjects) {
		DestroyObject(&obj);
	}

	SavedGameObjectTranslator translator;
	for (const auto& obj : data["objects"]) {
		auto newId = SpawnObject();
		translator.AddObject(GameObjectIndex(obj), newId);
	}

	// Systems
	m_systemManager->DeactivateAllSystems();

	for (const auto& [systemName, config] : data["systems"].items()) {
		auto* system = m_systemManager->GetSystem(systemName);

		if (auto* nSystem = dynamic_cast<ISerializableSystem*>(system)) {
			nSystem->ApplyConfiguration(config, translator);
		}

		m_systemManager->GetSystem(systemName)->Activate();
	}

	// Components
	for (const auto& componentEntry : data["components"]) {
		const std::string_view componentTypeName = componentEntry["component_type"];

		// Puede estar vacío
		if (!componentEntry.contains("components")) {
			continue;
		}

		UIndex64 componentId = 0;
		for (const auto& [objectId, componentData] : componentEntry["components"].items()) {
			const auto newObjectIdx = translator.GetCurrentIndex(GameObjectIndex(std::stoull(objectId)));

			auto* container = m_componentManager->GetContainer(componentTypeName);
			container->DeserializeComponent(componentData, newObjectIdx, translator);

			const auto componentType = container->GetComponentType();

			m_gameObjectManager->AddComponent(newObjectIdx, componentType);
			m_systemManager->GameObjectSignatureChanged(newObjectIdx, m_gameObjectManager->GetSignature(newObjectIdx));

			componentId++;
		}
	}

	return translator;
}

SavedGameObjectTranslator EntityComponentSystem::LoadBinaryScene(std::string_view path) {
	OSK_ASSERT(
		IO::FileIO::FileExists(path),
		InvalidArgumentException(std::format("EL archivo de escena {} no existe.", path)));

	auto data = IO::FileIO::ReadBinaryFromFile(path);
	auto bData = DynamicArray<TByte>::CreateReserved(data.GetSize());
	for (const auto byte : data) {
		bData.Insert(byte);
	}

	auto block = BinaryBlock::FromData(bData);

	auto reader = block.GetReader();

	// Desactivar todos los sistemas.
	m_systemManager->DeactivateAllSystems();

	// Destruir objetos.
	{
		auto oldObjects = DynamicArray<GameObjectIndex>::CreateReserved(m_gameObjectManager->GetAllLivingObjects().size());

		for (const auto& obj : m_gameObjectManager->GetAllLivingObjects()) {
			oldObjects.Insert(obj);
		}

		for (auto& obj : oldObjects) {
			DestroyObject(&obj);
		}
	}
	

	// Header
	const auto magicNumber = reader.Read<TByte>();
	OSK_ASSERT(magicNumber == 1, InvalidBinaryDeserializationException("El archivo no sigue el formato .bsf"));

	const auto specVersion = reader.Read<TByte>();
	OSK_ASSERT(specVersion == 0, InvalidBinaryDeserializationException("El archivo sigue una especificación inválida."));

	// Offsets de los grandes bloques.
	const auto systemBlockOffset = reader.Read<UIndex64>();
	const auto objectBlockOffset = reader.Read<UIndex64>();
	const auto componentBlockOffset = reader.Read<UIndex64>();

	OSK_ASSERT(
		componentBlockOffset < bData.GetSize(),
		InvalidBinaryDeserializationException("Offsets de grandes bloques incorrectos: superan el tamaño de bytes cagrados."));

	// Objects
	// Hacemos esto primero para poder construir el SavedGameObjectTranslator.
	SavedGameObjectTranslator translator{};
	{
		BinaryBlockReader objectReader = block.GetReader_WithOffset(objectBlockOffset);
		const auto numObjects = objectReader.Read<USize64>();

		for (UIndex64 i = 0; i < numObjects; i++) {
			translator.AddObject(
				// Leído en el archivo.
				GameObjectIndex(objectReader.Read<GameObjectIndex::TUnderlyingType>()), 
				// ID spawneado.
				SpawnObject());
		}
	}
	
	// Systems
	{
		auto systemReader = block.GetReader_WithOffset(systemBlockOffset);

		const auto numSistemas = systemReader.Read<USize64>();
		// Offset con los datos de los sistemas (primero van los nombres).
		const auto systemsDataOffset = systemReader.Read<USize64>() + systemReader.GetOriginalOffset();

		// Leemos los nombres cuyos datos han sido guardados.
		auto systemNames = DynamicArray<std::string>::CreateReserved(numSistemas);
		for (UIndex64 i = 0; i < numSistemas; i++) {
			systemNames.Insert(systemReader.ReadString());
		}

		// Leemos los datos de los sistemas.
		auto systemDatasReader = block.GetReader_WithOffset(systemsDataOffset);

		for (const auto& name : systemNames) {
			// Tamaño del bloque.
			// Para comprobar que se leen correctamente todos los datos.
			// Si no se leen todos los datos, se pueden leer datos corrompidos
			// en los siguientes sistemas.
			const auto blockSize = systemDatasReader.Read<USize64>();
			const auto currentOffset = systemDatasReader.GetCurrentIndex();

			// Leemos la información del sistema, y lo aplicamos.
			if (auto* nSystem = dynamic_cast<ISerializableSystem*>(m_systemManager->GetSystem(name))) {
				nSystem->ApplyConfiguration(&systemDatasReader, translator);
			}

			m_systemManager->GetSystem(name)->Activate();

			// Obtenemos el número de bytes leídos, y comprobamos que sea el mismo número
			// que los datos guardados.
			const auto newOffset = systemDatasReader.GetCurrentIndex();
			OSK_ASSERT(
				newOffset - currentOffset == blockSize, 
				InvalidBinaryDeserializationException(std::format("El componente o sistema{} no ha leído todos los datos de su bloque.", name)));
		}
	}

	// Components
	{
		auto componentReader = block.GetReader_WithOffset(componentBlockOffset);

		// Numero de TIPOS de componentes guardados.
		const auto numComponentTypes = componentReader.Read<USize64>();
		// Offset de los datos de los componentes (antes van los nombres).
		const auto componentDatasOffset = componentReader.Read<USize64>();

		// Leemos los nombres de los TIPOS de componentes.
		auto componentTypeNames = DynamicArray<std::string>::CreateReserved(numComponentTypes);
		for (UIndex64 i = 0; i < numComponentTypes; i++) {
			componentTypeNames.Insert(componentReader.ReadString());
		}

		// Offset inicial del bloque de datos de los componentes.
		const auto originalDataBlockOffset = componentDatasOffset + componentReader.GetOriginalOffset();
		// Número de bytes de datos leídos.
		USize64 readDataOffset = 0;

		for (const auto& componentTypeName : componentTypeNames) {
			// Offset del bloque con los datos de este tipo de componente.
			//	= offset del bloque de datos + número de bytes leídos en iteraciones anteriores.
			auto componentDataReader = block.GetReader_WithOffset(originalDataBlockOffset + readDataOffset);

			// Número de instancias guardadas.
			const auto numComponentes = componentDataReader.Read<USize64>();
			// Offset del bloque de IDs de objetos.
			const auto componentObjectsOffset = componentDataReader.Read<USize64>();


			// Lector de los objetos asociados a los componentes.
			auto componentObjectsReader = block.GetReader_WithOffset(componentObjectsOffset + originalDataBlockOffset + readDataOffset);

			if (numComponentes == 0) {
				goto osk_applyoffsets_and_continue;
			}

			for (UIndex64 i = 0; i < numComponentes; i++) {
				const auto newObjectIdx = translator.GetCurrentIndex(GameObjectIndex(componentObjectsReader.Read<GameObjectIndex::TUnderlyingType>()));
				OSK_ASSERT(m_gameObjectManager->IsGameObjectAlive(newObjectIdx), InvalidBinaryDeserializationException("Objeto enlazado a componente no válido."));

				auto* container = m_componentManager->GetContainer(componentTypeName);
				container->BinaryDeserializeComponent(&componentDataReader, newObjectIdx, translator);

				const auto componentType = container->GetComponentType();

				m_gameObjectManager->AddComponent(newObjectIdx, componentType);
				m_systemManager->GameObjectSignatureChanged(newObjectIdx, m_gameObjectManager->GetSignature(newObjectIdx));
			}

		osk_applyoffsets_and_continue:
			readDataOffset += 
				componentDataReader.GetCurrentIndex() - componentDataReader.GetOriginalOffset() +
				componentObjectsReader.GetCurrentIndex() - componentObjectsReader.GetOriginalOffset();
		}

		OSK_ASSERT(
			originalDataBlockOffset + readDataOffset == bData.GetSize(), 
			InvalidBinaryDeserializationException("No se han leído todos los datos guardados."));
	}	

	return translator;
}

void EntityComponentSystem::_ClearEventQueues() {
	m_eventManager->_ClearQueues();
}

ISystem* EntityComponentSystem::GetSystemByName(std::string_view name) {
	return m_systemManager->GetSystem(name);
}

DynamicArray<ComponentType> EntityComponentSystem::GetObjectComponentsTypes(GameObjectIndex obj) const {
	OSK_ASSERT(
		m_gameObjectManager->IsGameObjectAlive(obj),
		InvalidArgumentException(std::format("El objeto {} no existe.", obj.Get())));

	DynamicArray<ComponentType> output{};

	const auto& signature = m_gameObjectManager->GetSignature(obj);

	for (ComponentType index = 0; index < MAX_COMPONENT_TYPES; index++) {
		if (signature.Get(index)) {
			output.Insert(index);
		}
	}

	return output;
}

void* EntityComponentSystem::GetComponentAddress(GameObjectIndex obj, ComponentType type) {
	return m_componentManager->GetComponentAddress(obj, type);
}

const void* EntityComponentSystem::GetComponentAddress(GameObjectIndex obj, ComponentType type) const {
	return m_componentManager->GetComponentAddress(obj, type);
}

bool EntityComponentSystem::ObjectHasComponent(GameObjectIndex obj, ComponentType componentType) const {
	return m_componentManager->GetComponentAddress(obj, componentType) != nullptr;
}

std::string EntityComponentSystem::GetComponentTypeName(ComponentType type) const {
	return m_componentManager->GetComponentTypeName(type);
}

GameObjectIndex EntityComponentSystem::SpawnObject() {
	return m_gameObjectManager->CreateGameObject();
}

void EntityComponentSystem::DestroyObject(GameObjectIndex* obj) {
	m_systemManager->GameObjectDestroyed(*obj);
	m_componentManager->GameObjectDestroyed(*obj);
	m_gameObjectManager->DestroyGameObject(obj);
}

std::span<const GameObjectIndex> EntityComponentSystem::GetLivingObjects() const {
	return m_gameObjectManager->GetAllLivingObjects();
}

DynamicArray<const ISystem*> EntityComponentSystem::GetAllSystems() const {
	DynamicArray<const ISystem*> output{};
	const auto& graph = m_systemManager->GetExecutionGraph();

	for (const auto& set : graph.GetExecutionGraph()) {
		for (const auto& system : set.systems) {
			output.Insert(system);
		}
	}

	return output;
}

bool EntityComponentSystem::IsGameObjectAlive(GameObjectIndex obj) const {
	return m_gameObjectManager->IsGameObjectAlive(obj);
}

void EntityComponentSystem::EndFrame() {
	m_eventManager->_ClearQueues();
}

const DynamicArray<IRenderSystem*>& EntityComponentSystem::GetRenderSystems() const {
	return m_renderSystems;
}
