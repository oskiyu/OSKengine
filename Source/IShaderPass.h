#pragma once

#include "OSKmacros.h"

#include "Material.h"
#include "UniquePtr.hpp"
#include "MaterialInstance.h"
#include "GBuffer.h"

#include "DynamicArray.hpp"
#include "GameObject.h"

#include "MeshMapping.h"
#include "Texture.h"
#include "AssetRef.h"

#include "GameObject.h"

#include <unordered_map>


namespace OSK::GRAPHICS {

	class ICommandList;
	class GpuModel3D;
	class GpuMesh3D;
	class ShadowMap;


	/// @brief Representa un pase de renderizado de rasterizado
	/// que usa un shader en concreto.
	class OSKAPI_CALL IShaderPass {

	protected:

		explicit IShaderPass(const std::string& passTypeName) : m_passTypeName(passTypeName) {}

	public:

		OSK_DEFINE_AS(IShaderPass);

		virtual ~IShaderPass() = default;

		/// @brief Carga los shaders del pase.
		virtual void Load() = 0;

		/// @brief Actualiza el MeshMapping local.
		/// @param objectsToRender Objetos a renderizar.
		void UpdateLocalMeshMapping(const DynamicArray<ECS::GameObjectIndex>& objectsToRender);

		/// @brief Realiza el bucle de renderizado del pase.
		/// @param commandList Lista de comandos gráficos.
		/// @param objectsToRender Objetos a renderizar.
		/// @param jitterIndex Índice del jitter de TAA de este frame.
		/// @param resolution Resolución del GBuffer en este frame.
		virtual void RenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			GlobalMeshMapping* globalMeshMapping,
			UIndex32 jitterIndex,
			Vector2ui resolution) {}

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
			const GpuModel3D& model,
			const GpuMesh3D& mesh);

		LocalMeshMapping m_localMeshMapping{};

		/// @brief Resolución actual del GBuffer.
		Vector2ui m_currentResolution = Vector2ui::Zero;

		/// @brief Material del pase.
		Material* m_passMaterial = nullptr;

		/// @brief Instancia del pase.
		UniquePtr<MaterialInstance> m_materialInstance = nullptr;

		/// @brief Camara de la escena.
		ECS::GameObjectIndex m_cameraObject = ECS::EMPTY_GAME_OBJECT;

		/// @brief Nombre del pase.
		std::string m_passTypeName = "";

	};


	/// @brief Pase de renderizado de sombras.
	class IShadowsPass : public IShaderPass {

	protected:

		explicit IShadowsPass(const std::string& passTypeName) : IShaderPass(passTypeName) {}

	public:

		OSK_DEFINE_AS(IShadowsPass);

		virtual ~IShadowsPass() = default;

		virtual void SetupShadowMap(const ShadowMap& shadowMap) = 0;

		virtual void ShadowsRenderLoop(
			ICommandList* commandList,
			const DynamicArray<ECS::GameObjectIndex>& objectsToRender,
			UIndex32 cascadeIndex,
			const ShadowMap& map) {}

	};


	/// @brief Almacena los pases de renderizado de un sistema de renderizado,
	/// además de los objetos compatibles con cada paso.
	class ShaderPassTable {

	public:

		/// @brief Añade un nuevo pase a la tabla.
		/// @param pass Nuevo pase.
		/// 
		/// @pre No debe haber un pase con el mimso nombre dentro de la tabla.
		void AddShaderPass(OwnedPtr< GRAPHICS::IShaderPass> pass);

		/// @brief Elimina un pase de la tabla.
		/// @param passName Nombre del pase.
		/// 
		/// @pre Debe haber un pase dentro de la tabla con el nombre @p passName.
		void RemoveShaderPass(std::string_view passName);

		/// @brief Elimina todos los pases.
		void RemoveAllPasses();

		/// @return True si la tabla contiene un pase con el nombre @p passName.
		bool ContainsShaderPass(std::string_view passName) const;


		/// @param passName Nombre del pase.
		/// @return Pase con el nombre dado.
		/// @pre Debe haber un pase dentro de la tabla con el nombre @p passName.
		GRAPHICS::IShaderPass* GetShaderPass(std::string_view passName);
		const GRAPHICS::IShaderPass* GetShaderPass(std::string_view passName) const;


		/// @brief Registra un objeto como compatible con un pase en concreto.
		/// @param passName Nombre del pase.
		/// @param obj ID del objeto compatible.
		/// 
		/// @pre Debe haber un pase dentro de la tabla con el nombre @p passName.
		/// @pre @p obj debe ser un ID válido.
		/// @pre @p obj no debe haber sido previamente añadido al pase indicado.
		void AddCompatibleObject(std::string_view passName, ECS::GameObjectIndex obj);

		/// @brief Elimina un objeto como compatible con un pase en concreto.
		/// @param passName Nombre del pase.
		/// @param obj ID del objeto compatible.
		/// 
		/// @pre Debe haber un pase dentro de la tabla con el nombre @p passName.
		/// @pre @p obj debe haber sido previamente añadido al pase indicado.
		void RemoveCompatibleObject(std::string_view passName, ECS::GameObjectIndex obj);


		/// @param passName Nombre del pase.
		/// @return Lista con los objetos compatibles con el pase.
		/// @pre Debe haber un pase dentro de la tabla con el nombre @p passName.
		DynamicArray<ECS::GameObjectIndex>& GetCompatibleObjects(std::string_view passName);
		const DynamicArray<ECS::GameObjectIndex>& GetCompatibleObjects(std::string_view passName) const;


		/// @return Todos los pases almacenados en la tabla.
		DynamicArray<GRAPHICS::IShaderPass*>& GetAllPasses();

		/// @return Todos los pases almacenados en la tabla.
		std::span<GRAPHICS::IShaderPass* const> GetAllPasses() const;

	private:

		/// @brief Pases de renderizado.
		DynamicArray<UniquePtr<GRAPHICS::IShaderPass>> m_renderPasses;
		DynamicArray<GRAPHICS::IShaderPass*> m_renderPassesPtrs;

		/// @brief Mapa nombre -> pase de renderizado.
		std::unordered_map<std::string, GRAPHICS::IShaderPass*, StringHasher, std::equal_to<>> m_renderPassesTable;

		/// @brief Objetos compatibles con cada pase.
		std::unordered_map<std::string, DynamicArray<ECS::GameObjectIndex>, StringHasher, std::equal_to<>> m_objectsPerPass;

	};

}

#ifndef OSK_RENDERPASS
#define OSK_RENDERPASS(classTypeName, className) constexpr static inline std::string_view GetRenderPassName() { return className; } \
static ::OSK::OwnedPtr<::OSK::GRAPHICS::IShaderPass> CreateInstance() { return new classTypeName; }
#endif
