#pragma once

#include "OSKmacros.h"

#include "Material.h"
#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "GBuffer.h"

#include "DynamicArray.hpp"
#include "GameObject.h"

#include "MeshMapping.h"

#include <unordered_map>

namespace OSK::ASSETS{
	class Model3D;
}

namespace OSK::GRAPHICS {

	class ICommandList;
	class Mesh3D;


	/// @brief Representa un pase de renderizado.
	class OSKAPI_CALL IRenderPass {

	protected:

		explicit IRenderPass(const std::string& passTypeName) : m_passTypeName(passTypeName) {}

	public:

		OSK_DEFINE_AS(IRenderPass);

		virtual ~IRenderPass() = default;

		/// @brief Carga los materiales del pase.
		virtual void Load() = 0;

		/// @brief Realiza el bucle de renderizado del pase.
		/// @param commandList Lista de comandos gráficos.
		/// @param objectsToRender Objetos a renderizar.
		/// @param jitterIndex Índice del jitter de TAA de este frame.
		/// @param resolution Resolución del GBuffer en este frame.
		virtual void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 jitterIndex,
			Vector2ui resolution) = 0;

		/// @brief Establece la cámara de la escena 3D.
		/// @param cameraObject Objeto de la cámara.
		void SetCamera(ECS::GameObjectIndex cameraObject);


		/// @return Nombre del pase.
		std::string_view GetTypeName() const;


		/// @return Instancia del resolver.
		MaterialInstance* GetMaterialInstance() { return m_materialInstance.GetPointer(); }

		/// @return Instancia del resolver.
		const MaterialInstance* GetMaterialInstance() const { return m_materialInstance.GetPointer(); }

	protected:

		/// @brief Establece la instancia del material para un mesh dado.
		/// Por defecto, usa el modelo PBR.
		/// @param model Modelo que posee el mesh.
		/// @param mesh Mesh a establecer.
		virtual void SetupMaterialInstance(
			const ASSETS::Model3D& model,
			const Mesh3D& mesh);


		/// @brief Resolución actual del GBuffer.
		Vector2ui m_currentResolution = Vector2ui::Zero;

		/// @brief Material del pase.
		Material* m_passMaterial = nullptr;

		/// @brief Instancia del pase.
		UniquePtr<MaterialInstance> m_materialInstance = nullptr;

		/// @brief Camara de la escena.
		ECS::GameObjectIndex m_cameraObject = ECS::EMPTY_GAME_OBJECT;

		/// @brief Matrices de los modelos en el fotograma anterior.
		std::unordered_map<ECS::GameObjectIndex, glm::mat4> m_previousModelMatrices;

		/// @brief Nombre del pase.
		std::string m_passTypeName = "";

		MeshMapping m_meshMapping = {};

	};

}

#ifndef OSK_RENDERPASS
#define OSK_RENDERPASS(className) constexpr static inline std::string_view GetRenderPassName() { return className; }
#endif
