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
	/// Implementa un dise�o bindless unido a
	/// renderizado instancing, que reduce
	/// enormemente el n�mero de llamadas
	/// de renderizado a ejecutar en la GPU.
	class OSKAPI_CALL SdfBindlessRenderer2D {

	public:

		/// @brief Ruta al archivo de material por defecto
		/// para este tipo de renderizado.
		constexpr static auto DefaultMaterialPath = "Resources/Materials/2D/sdf_material_2d_bindless.json";


		/// @brief Crea el renderizador.
		/// @param ecs Clase principal ECS del motor.
		/// @param allocator Alojador de memoria a usar.
		/// @param defaultMaterial Material que se usar�
		/// por defecto (se podr� reestablecer m�s
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

		/// @brief Establece el material que se usar� sobre los siguientes
		/// elementos.
		/// @param material Material a usar.
		void SetMaterial(Material* material);


		/// @brief Establece la c�mara 2D con la que se renderizar�n los pr�ximos sprites.
		/// @param camera C�mara 2D.
		/// @param cameraTransform Transform de la c�mara.
		void SetCamera(const ECS::CameraComponent2D& camera, const ECS::Transform2D& cameraTransform);

		/// @brief Establece la c�mara 2D con la que se renderizar�n los pr�ximos sprites.
		/// @param gameObject ID de un objeto con una c�mara 2D.
		/// @pre @p gameObject debe tener un componente del tipo CameraComponent2D.
		/// @pre @p gameObject debe tener un componente del tipo Transform2D.
		void SetCamera(ECS::GameObjectIndex gameObject);

#pragma endregion

#pragma region Drawing

		/// @brief Comienza el proceso de renderizado.
		/// @param commandList Lista de comandos sobre
		/// la que se grabar�n los comandos.
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

		/// @param text Informaci�n de un texto a
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

		/// @brief Crea un nuevo batch y lo a�ade
		/// a la lista de batches.
		/// @param material Material del batch.
		/// @param resourceIndex �ndice del recurso del fotograma.
		/// 
		/// @pre @p material no es null.
		/// 
		/// @post Hay un nuevo batch al final de `m_drawCallsBuffers`.
		/// @post Hay un nuevo buffer inicializado al final de `m_drawCallsBuffers`.
		/// @post Hay una nueva instancia inicializada al final de `m_drawCallsBuffersInstances`.
		/// @post El anterior �ltimo elemento de `m_drawCallsBuffersInstances`
		/// se actualiza (Flush).
		/// 
		/// @note No actualiza los �ndices m_currentBatchIndex y m_insideBatchIndex.
		void CreateNewBatch(const Material* material, UIndex32 resourceIndex);

		/// @brief Crea un nuevo buffer para almacenar
		/// la informaci�n de los elementos a renderizar.
		/// @param resourceIndex �ndice del recurso del fotograma.
		/// 
		/// @post Hay un nuevo buffer inicializado al final de `m_drawCallsBuffers`.
		/// @post Hay una nueva instancia al final de `m_drawCallsBuffersInstances`.
		void AddContentBuffer(UIndex32 resourceIndex);

		/// @brief Rellena la informaci�n de la instancia de material
		/// para a�adir la informaci�n de un elemento.
		/// @param buffer Buffer que contendr� la informaci�n
		/// del elemento.
		/// @param imageView (Opcional) textura del elemento.
		/// @param range Rango del buffer a usar.
		/// @param resourceIndex �ndice del recurso del fotograma.
		/// 
		/// @pre @p range.size == `sizeof(SdfBindlessBufferContent2D)`.
		void EnsureMaterialSlotContent(
			const GpuBuffer& buffer,
			const IGpuImageView* imageView,
			const GpuBufferRange& range,
			UIndex32 resourceIndex);

		/// @return Slot que contiene la informaci�n
		/// global del renderizador.
		/// @invariant No es null.
		IMaterialSlot* GetGlobalInformationSlot(UIndex32 resourceIndex);

		/// @return Cantidad de padding que debe tener 
		/// un buffer de instancia para cumplir con
		/// las necesidades de alineamiento.
		USize64 GetDrawCallBufferContentPadding() const;


		/// @brief Matriz actual de la c�mara.
		glm::mat4 m_currentCameraMatrix = glm::mat4(1.0f);

		/// @brief Lista de comandos a usar para
		/// renderizado.
		/// @invariant Si el renderizador est� abierto, no ser� null.
		/// @invariant Si el renderizador est� cerrado, ser� null.
		ICommandList* m_targetCommandList = nullptr;

		/// @brief Material a usar en los siguientes batches.
		/// Puede ser sobreescrito mediante `SetMaterial()`.
		/// @invariant No es null.
		Material* m_material = nullptr;

		/// @brief Batches a renderizar.
		/// 
		/// Se ir� rellenando a medida que se
		/// rendericen elementos.
		/// 
		/// Al finalizar el renderizado (`End()`),
		/// se procesan para grabar la lista de
		/// comandos y se vaciar�.
		DynamicArray<SdfBindlessBatch2D> m_batches{};

		/// @brief Contendr�:
		/// - Resoluci�n de la pantalla, en p�xeles.
		std::array<UniquePtr<GpuBuffer>, MAX_RESOURCES_IN_FLIGHT> m_globalInformationBuffers{};

		/// @brief Contendr� el slot de informaci�n global.
		std::array<UniquePtr<MaterialInstance>, MAX_RESOURCES_IN_FLIGHT> m_globalInformationInstances{};


		/// @brief Lista de buffers que almacenar�n
		/// los datos de las instancias.
		/// 
		/// Cada buffer contiene como mucho `MAX_DRAW_CALLS_PER_BATCH`
		/// instancias.
		std::array<DynamicArray<UniquePtr<GpuBuffer>>, MAX_RESOURCES_IN_FLIGHT> m_drawCallsBuffers;

		/// @brief Lista con las instancias configuradas de tal
		/// manera que a cada una le corresponda un buffer
		/// de `m_drawCallsBuffers`.
		std::array<DynamicArray<UniquePtr<MaterialInstance>>, MAX_RESOURCES_IN_FLIGHT> m_drawCallsBuffersInstances;

		/// @brief �ndice del batch que se est�
		/// procesando en un momento dado.
		/// 
		/// Se resetea cada frame.
		/// 
		/// Tambi�n indica:
		/// - Buffer de `m_drawCallsBuffers` que se est� rellenando.
		/// - Slot de `m_drawCallsBuffersInstances` que se est� configurando.
		UIndex32 m_currentBatchIndex = 0;

		/// @brief �ndice de la instancia dentro de su batch.
		/// Se resetea cada frame.
		UIndex32 m_insideBatchIndex = 0;

		IGpuMemoryAllocator* m_memoryAllocator = nullptr;
		ECS::EntityComponentSystem* m_ecs = nullptr;

	};

}
