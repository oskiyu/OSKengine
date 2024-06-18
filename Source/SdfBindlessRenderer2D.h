#pragma once

#include "ApiCall.h"

#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "Vector2.hpp"

#include "GameObject.h"

#include "MaterialInstance.h"
#include "GpuBuffer.h"

#include "SdfBindlessBatch2D.h"
#include <array>
#include "ResourcesInFlight.h"

namespace OSK::ECS {
	class EntityComponentSystem;
	class CameraComponent2D;
	class Transform2D;
}

namespace OSK::GRAPHICS {

	class IGpuMemoryAllocator;
	class ICommandList;

	class Material;
	class IMaterialSlot;

	struct SdfDrawCall2D;
	struct SdfStringInfo;


	/// @brief Clase para el renderizado 2D.
	/// Permite usar tanto sprites como formas
	/// definidas mediante signed-field distances.
	/// 
	/// Implementa un diseño bindless unido a
	/// renderizado instancing, que reduce
	/// enormemente el número de llamadas
	/// de renderizado a ejecutar en la GPU.
	class OSKAPI_CALL SdfBindlessRenderer2D {

	public:

		/// @brief Ruta al archivo de material por defecto
		/// para este tipo de renderizado.
		constexpr static auto DefaultMaterialPath = "Resources/Materials/2D/sdf_material_2d_bindless.json";


		/// @brief Crea el renderizador.
		/// @param ecs Clase principal ECS del motor.
		/// @param allocator Alojador de memoria a usar.
		/// @param defaultMaterial Material que se usará
		/// por defecto (se podrá reestablecer más
		/// adelante).
		/// 
		/// @pre @p ecs no es null.
		/// @pre @p allocator no es null.
		/// @pre @p defaultMaterial no es null.
		SdfBindlessRenderer2D(
			ECS::EntityComponentSystem* ecs,
			IGpuMemoryAllocator* allocator,
			Material* defaultMaterial);

#pragma region Inicializadores

		/// @brief Establece el material que se usará sobre los siguientes
		/// elementos.
		/// @param material Material a usar.
		void SetMaterial(Material* material);


		/// @brief Establece la cámara 2D con la que se renderizarán los próximos sprites.
		/// @param camera Cámara 2D.
		/// @param cameraTransform Transform de la cámara.
		void SetCamera(const ECS::CameraComponent2D& camera, const ECS::Transform2D& cameraTransform);

		/// @brief Establece la cámara 2D con la que se renderizarán los próximos sprites.
		/// @param gameObject ID de un objeto con una cámara 2D.
		/// @pre @p gameObject debe tener un componente del tipo CameraComponent2D.
		/// @pre @p gameObject debe tener un componente del tipo Transform2D.
		void SetCamera(ECS::GameObjectIndex gameObject);

#pragma endregion

#pragma region Drawing

		/// @brief Comienza el proceso de renderizado.
		/// @param commandList Lista de comandos sobre
		/// la que se grabarán los comandos.
		/// 
		/// @pre @p commandList debe soportar, al menos,
		/// `CommandsSupport::GRAPHICS`.
		/// 
		/// @pre El renderizador debe estar cerrado,
		/// que es el estado por defecto.
		/// 
		/// @post El renderizador pasa a estar abierto.
		void Begin(ICommandList* commandList);

		/// @brief Renderiza el elemento indicado.
		/// @param drawCall Propiedades del elemento
		/// a renderizar.
		/// 
		/// @pre El renderizador debe estar abierto
		/// (debe haberse llamado previamente a Begin).
		void Draw(const SdfDrawCall2D& drawCall);

		/// @brief Renderiza el texto indicado.
		/// @param stringCall Propiedades del texto
		/// a renderizar.
		/// 
		/// @pre El renderizador debe estar abierto
		/// (debe haberse llamado previamente a Begin).
		void Draw(const SdfStringInfo& stringCall);

		/// @brief Finaliza el proceso de renderizado.
		///
		/// @pre El renderizador debe estar abierto
		/// (debe haberse llamado previamente a Begin).
		/// 
		/// @post El renderizador pasa a estar cerrado.
		void End();

#pragma endregion

		/// @param text Información de un texto a
		/// renderizar.
		/// @return Lista de llamadas de renderizado necesarias
		/// para renderizar el texto indicado.
		static DynamicArray<SdfDrawCall2D> GetTextDrawCalls(const SdfStringInfo& text);

	private:

		constexpr static USize64 MAX_DRAW_CALLS_PER_BATCH = 512;

		constexpr static auto GlobalMaterialSlotName = "global";
		constexpr static auto GlobalMaterialBindingName = "global";

		constexpr static auto DrawCallsSlotName = "drawCalls";
		constexpr static auto DrawCallsBindingName = "drawCalls";
		constexpr static auto DrawCallsImageBindingName = "images";

		/// @brief Crea un nuevo batch y lo añade
		/// a la lista de batches.
		/// @param material Material del batch.
		/// @param resourceIndex Índice del recurso del fotograma.
		/// 
		/// @pre @p material no es null.
		/// 
		/// @post Hay un nuevo batch al final de `m_drawCallsBuffers`.
		/// @post Hay un nuevo buffer inicializado al final de `m_drawCallsBuffers`.
		/// @post Hay una nueva instancia inicializada al final de `m_drawCallsBuffersInstances`.
		/// @post El anterior último elemento de `m_drawCallsBuffersInstances`
		/// se actualiza (Flush).
		/// 
		/// @note No actualiza los índices m_currentBatchIndex y m_insideBatchIndex.
		void CreateNewBatch(const Material* material, UIndex32 resourceIndex);

		/// @brief Crea un nuevo buffer para almacenar
		/// la información de los elementos a renderizar.
		/// @param resourceIndex Índice del recurso del fotograma.
		/// 
		/// @post Hay un nuevo buffer inicializado al final de `m_drawCallsBuffers`.
		/// @post Hay una nueva instancia al final de `m_drawCallsBuffersInstances`.
		void AddContentBuffer(UIndex32 resourceIndex);

		/// @brief Rellena la información de la instancia de material
		/// para añadir la información de un elemento.
		/// @param buffer Buffer que contendrá la información
		/// del elemento.
		/// @param imageView (Opcional) textura del elemento.
		/// @param range Rango del buffer a usar.
		/// @param resourceIndex Índice del recurso del fotograma.
		/// 
		/// @pre @p range.size == `sizeof(SdfBindlessBufferContent2D)`.
		void EnsureMaterialSlotContent(
			const GpuBuffer& buffer,
			const IGpuImageView* imageView,
			const GpuBufferRange& range,
			UIndex32 resourceIndex);

		/// @return Slot que contiene la información
		/// global del renderizador.
		/// @invariant No es null.
		IMaterialSlot* GetGlobalInformationSlot(UIndex32 resourceIndex);

		/// @return Cantidad de padding que debe tener 
		/// un buffer de instancia para cumplir con
		/// las necesidades de alineamiento.
		USize64 GetDrawCallBufferContentPadding() const;


		/// @brief Matriz actual de la cámara.
		glm::mat4 m_currentCameraMatrix = glm::mat4(1.0f);

		/// @brief Lista de comandos a usar para
		/// renderizado.
		/// @invariant Si el renderizador está abierto, no será null.
		/// @invariant Si el renderizador está cerrado, será null.
		ICommandList* m_targetCommandList = nullptr;

		/// @brief Material a usar en los siguientes batches.
		/// Puede ser sobreescrito mediante `SetMaterial()`.
		/// @invariant No es null.
		Material* m_material = nullptr;

		/// @brief Batches a renderizar.
		/// 
		/// Se irá rellenando a medida que se
		/// rendericen elementos.
		/// 
		/// Al finalizar el renderizado (`End()`),
		/// se procesan para grabar la lista de
		/// comandos y se vaciará.
		DynamicArray<SdfBindlessBatch2D> m_batches{};

		/// @brief Contendrá:
		/// - Resolución de la pantalla, en píxeles.
		std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> m_globalInformationBuffers{};

		/// @brief Contendrá el slot de información global.
		std::array<UniquePtr<MaterialInstance>, MAX_RESOURCES_IN_FLIGHT> m_globalInformationInstances{};


		/// @brief Lista de buffers que almacenarán
		/// los datos de las instancias.
		/// 
		/// Cada buffer contiene como mucho `MAX_DRAW_CALLS_PER_BATCH`
		/// instancias.
		std::array<DynamicArray<UniquePtr<GpuBuffer>>, MAX_RESOURCES_IN_FLIGHT> m_drawCallsBuffers;

		/// @brief Lista con las instancias configuradas de tal
		/// manera que a cada una le corresponda un buffer
		/// de `m_drawCallsBuffers`.
		std::array<DynamicArray<UniquePtr<MaterialInstance>>, MAX_RESOURCES_IN_FLIGHT> m_drawCallsBuffersInstances;

		/// @brief Índice del batch que se está
		/// procesando en un momento dado.
		/// 
		/// Se resetea cada frame.
		/// 
		/// También indica:
		/// - Buffer de `m_drawCallsBuffers` que se está rellenando.
		/// - Slot de `m_drawCallsBuffersInstances` que se está configurando.
		UIndex32 m_currentBatchIndex = 0;

		/// @brief Índice de la instancia dentro de su batch.
		/// Se resetea cada frame.
		UIndex32 m_insideBatchIndex = 0;

		IGpuMemoryAllocator* m_memoryAllocator = nullptr;
		ECS::EntityComponentSystem* m_ecs = nullptr;

	};

}
