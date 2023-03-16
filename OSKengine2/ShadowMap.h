#pragma once

#include "RenderTarget.h"
#include "IGpuUniformBuffer.h"
#include "MaterialInstance.h"
#include "Vector4.hpp"
#include "GameObject.h"

namespace OSK::ASSETS {
	enum class ModelType;
}

namespace OSK::GRAPHICS {

	struct DirectionalLight;

	/// @brief 
	/// Renderiza un mapa de profundidad de la escena desde el punto de
	/// vista de una luz direccional.
	/// 
	/// Este mapa de profundidad puede usarse para proyectar las sombras
	/// de la luz direccional dentro de una escena.
	/// 
	/// Para usar un ShadowMap, en el bucle de renderizado:
	///		- Iniciar un renderpass con el render target ShadowMap::GetShadowsRenderTarget.
	///		- Enlazar el material ShadowMap::GetShadowsMaterial.
	///		- Enlazar la instancia de material ShadowMap::GetShadowsMaterialInstance.
	///		- Renderizar todos los objetos que deban proyectar sombras.
	///		- Finalizar el renderpass.
	class OSKAPI_CALL ShadowMap {

	public:

		/// @brief Inicializa el shadow map.
		/// @param imageSize Resoluci�n de la imagen del shadow map.
		/// Cuanto m�s alta, mayor calidad de sombras.
		void Create(const Vector2ui& imageSize);


		/// @brief Establece la luz direccional que proyectar� las sobras
		/// sobre este mapa de sombras.
		/// 
		/// @pre Debe llamarse despu�s de inicializar el mapa de sombras (ShadowMap::Create).
		void SetDirectionalLight(const DirectionalLight& dirLight);


		/// @brief Establece el near plane del frustum de renderizado.
		/// @param nearPlane Distancia m�nima al origen
		/// de la luz para que sean renderizados.
		/// 
		/// @note Por defecto, -5.0f.
		void SetNearPlane(float nearPlane);

		/// @brief Establece el far plane del frustum de renderizado.
		/// @param farPlane Distancia m�xima al origen
		/// de la luz para que sean renderizados.
		/// 
		/// @note Por defecto, 5.0f.
		void SetFarPlane(float farPlane);

				
		/// @brief Devuelve la imagen del mapa de sombras.
		/// @param index �ndice de la imagen en el swapchain.
		/// @return Mapa de profundidad.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolver� nullptr si no se cumple la precondici�n.
		GpuImage* GetShadowImage(TSize index) const;
		
		/// @brief Devuelve la imagen de color del mapa de sombras.
		/// Para su renderizado.
		/// @param index �ndice de la imagen en el swapchain.
		/// @return Mapa de profundidad.
		GpuImage* GetColorImage(TSize index) const;

				
		/// @brief Devuelve el material usado para la generaci�n de sombras.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolver� nullptr si no se cumple la precondici�n.
		Material* GetShadowsMaterial(ASSETS::ModelType modelType) const;

		/// @brief Devuelve la instancia del material para la generaci�n de sombras.
		///
		/// @note Se debe vincular el slot "global" para poder ser usado correctamente.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolver� nullptr si no se cumple la precondici�n.
		MaterialInstance* GetShadowsMaterialInstance() const;

		/// @brief Devuelve el uniform buffer que contiene la matriz de
		/// proyecci�n y vista de la luz que proyecta la sombra.
		/// 
		/// @pre Se ha llamado a ShadowMap::Create.
		/// @warning Devuelve nullptr si no se cumple las precondiciones.
		DynamicArray<GRAPHICS::IGpuUniformBuffer*> GetDirLightMatrixUniformBuffers() const;


		/// @brief Establece la c�mara de la escena, para poder configurar
		/// los mapas de profundidad de una manera m�s �ptima.
		/// @param cameraObject Objeto ECS que contiene la c�mara.
		/// 
		/// @pre cameraObject debe tener asociado un componente de tipo ECS::CameraComponent3D.
		void SetSceneCamera(ECS::GameObjectIndex cameraObject);


		/// @return N�mero de mapas de sobras en cascada.
		TSize GetNumCascades() const;

	private:

		static DynamicArray<Vector3f> GetFrustumCorners(const glm::mat4& cameraMatrix);

		void UpdateLightMatrixBuffer();

		UniquePtr<GpuImage> unusedColorArrayAttachment[NUM_RESOURCES_IN_FLIGHT]{};
		UniquePtr<GpuImage> depthArrayAttachment[NUM_RESOURCES_IN_FLIGHT]{};

		Vector2f lightArea = Vector2f(5.f, 5.f);
		float nearPlane = -5.0f;
		float farPlane = 5.0f;

		Vector3f lightDirection = Vector3f(0.0f, 1.0f, 0.0f);
		Vector3f lightOrigin = Vector3f(0.0f);

		Material* shadowsGenMaterial = nullptr;
		Material* shadowsGenAnimMaterial = nullptr;
		UniquePtr<MaterialInstance> shadowsGenMaterialInstance = nullptr;

		UniquePtr<IGpuUniformBuffer> lightUniformBuffer[NUM_RESOURCES_IN_FLIGHT]{};

		const static TSize numMaps = 4;

		ECS::GameObjectIndex cameraObject = ECS::EMPTY_GAME_OBJECT;

		struct ShadowsBufferContent {
			alignas(16) glm::mat4 matrices[4];
			alignas(16) Vector4f cascadeSplits;
		};

	};

}
