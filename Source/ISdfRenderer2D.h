#pragma once

#include "ApiCall.h"
#include "DynamicArray.hpp"
#include "SdfDrawCall2D.h"


namespace OSK::ECS {
	class CameraComponent2D;
	class EntityComponentSystem;
}

namespace OSK::GRAPHICS {

	class ICommandList;
	struct SdfDrawCall2D;
	struct SdfStringInfo;
	class Material;

	/// @brief Clase para el renderizado 2D.
	/// Permite usar tanto sprites como formas
	/// definidas mediante signed-field distances.
	class OSKAPI_CALL ISdfRenderer2D {

	public:

		virtual ~ISdfRenderer2D() = default;

#pragma region Inicializadores

		/// @brief Establece el material que se usará sobre los siguientes
		/// elementos.
		/// @param material Material a usar.
		virtual void SetMaterial(Material* material);


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
		virtual void Begin(ICommandList* commandList);

		/// @brief Renderiza el elemento indicado.
		/// @param drawCall Propiedades del elemento
		/// a renderizar.
		/// 
		/// @pre El renderizador debe estar abierto
		/// (debe haberse llamado previamente a Begin).
		virtual void Draw(const SdfDrawCall2D& drawCall) = 0;

		/// @brief Renderiza el texto indicado.
		/// @param stringCall Propiedades del texto
		/// a renderizar.
		/// 
		/// @pre El renderizador debe estar abierto
		/// (debe haberse llamado previamente a Begin).
		virtual void Draw(const SdfStringInfo& stringCall) = 0;

		/// @brief Finaliza el proceso de renderizado.
		///
		/// @pre El renderizador debe estar abierto
		/// (debe haberse llamado previamente a Begin).
		/// 
		/// @post El renderizador pasa a estar cerrado.
		virtual void End();

#pragma endregion

		/// @param text Información de un texto a
		/// renderizar.
		/// @return Lista de llamadas de renderizado necesarias
		/// para renderizar el texto indicado.
		static DynamicArray<SdfDrawCall2D> GetTextDrawCalls(const SdfStringInfo& text);

	protected:

		explicit ISdfRenderer2D(ECS::EntityComponentSystem* ecs);

		glm::mat4 GetCurrentCameraMatrix() const;
		ICommandList* GetCurrentCommandList();
		Material* GetCurrentMaterial();

	private:

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

		ECS::EntityComponentSystem* m_ecs;

	};

}
