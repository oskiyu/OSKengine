#pragma once

#include "RenderTarget.h"
#include "GpuBuffer.h"
#include "MaterialInstance.h"
#include "Vector4.hpp"
#include "GameObject.h"

#include <array>

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
		/// @param imageSize Resolución de la imagen del shadow map.
		/// Cuanto más alta, mayor calidad de sombras.
		void Create(const Vector2ui& imageSize);


		/// @brief Establece la luz direccional que proyectará las sobras
		/// sobre este mapa de sombras.
		/// 
		/// @pre Debe llamarse después de inicializar el mapa de sombras (ShadowMap::Create).
		/// 
		/// @throws InvalidObjectStateException Si se incumple alguna rpecondición.
		void SetDirectionalLight(const DirectionalLight& dirLight);


		/// @brief Establece el near plane del frustum de renderizado.
		/// @param nearPlane Distancia mínima al origen
		/// de la luz para que sean renderizados.
		/// 
		/// @note Por defecto, -5.0f.
		void SetNearPlane(float nearPlane);

		/// @brief Establece el far plane del frustum de renderizado.
		/// @param farPlane Distancia máxima al origen
		/// de la luz para que sean renderizados.
		/// 
		/// @note Por defecto, 5.0f.
		void SetFarPlane(float farPlane);

				
		/// @brief Devuelve la imagen del mapa de sombras.
		/// @param index Índice de la imagen en el swapchain.
		/// @return Mapa de profundidad.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolverá nullptr si no se cumple la precondición.
		GpuImage* GetShadowImage();
		
		/// @brief Devuelve la imagen de color del mapa de sombras.
		/// Para su renderizado.
		/// @param index Índice de la imagen en el swapchain.
		/// @return Mapa de profundidad.
		GpuImage* GetColorImage();

				
		/// @brief Devuelve el material usado para la generación de sombras.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolverá nullptr si no se cumple la precondición.
		Material* GetShadowsMaterial(ASSETS::ModelType modelType);

		/// @brief Devuelve la instancia del material para la generación de sombras.
		///
		/// @note Se debe vincular el slot "global" para poder ser usado correctamente.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolverá nullptr si no se cumple la precondición.
		MaterialInstance* GetShadowsMaterialInstance();

		/// @brief Devuelve el uniform buffer que contiene la matriz de
		/// proyección y vista de la luz que proyecta la sombra.
		/// 
		/// @pre Se ha llamado a ShadowMap::Create.
		/// @warning Devuelve nullptr si no se cumple las precondiciones.
		DynamicArray<GRAPHICS::GpuBuffer*> GetDirLightMatrixUniformBuffers();


		/// @brief Establece la cámara de la escena, para poder configurar
		/// los mapas de profundidad de una manera más óptima.
		/// @param cameraObject Objeto ECS que contiene la cámara.
		/// 
		/// @pre cameraObject debe tener asociado un componente de tipo ECS::CameraComponent3D.
		void SetSceneCamera(ECS::GameObjectIndex cameraObject);


		/// @return Número de mapas de sobras en cascada.
		USize32 GetNumCascades() const;


		/// @brief Establece las distancias en las que se cambia el nivel de sombras.
		/// - Nivel 0: de 0 a splits[0].
		/// - Nivel 1: de splits[0] a splits[1].
		/// - Nivel 2: de splits[1] a splits[2].
		/// - Nivel 3: de splits[2] a splits[3].
		/// @param splits Límites de los niveles.
		void SetSplits(const std::array<float, 4>& splits);

	private:

		static DynamicArray<Vector3f> GetFrustumCorners(const glm::mat4& cameraMatrix);

		void UpdateLightMatrixBuffer();

		const static USize32 numMaps = 4;


		// --- Attachmens & buffers --- //

		UniquePtr<GpuImage> m_unusedColorArrayAttachment{};
		UniquePtr<GpuImage> m_depthArrayAttachment{};

		std::array<UniquePtr<GpuBuffer>, NUM_RESOURCES_IN_FLIGHT> lightUniformBuffer{};

		struct ShadowsBufferContent {
			alignas(16) glm::mat4 matrices[4];
			alignas(16) Vector4f cascadeSplits;
		};


		// --- Materials --- //

		Material* m_shadowsGenMaterial = nullptr;
		Material* m_shadowsGenAnimMaterial = nullptr;

		UniquePtr<MaterialInstance> m_shadowsGenMaterialInstance = nullptr;


		// --- Light & camera info --- //

		ECS::GameObjectIndex m_cameraObject = ECS::EMPTY_GAME_OBJECT;


		Vector4f m_splitsVec = Vector4f(5.0f, 50.0f, 500.0f, 5000.0f);
		std::array<float, 4> m_splits = { 5.0f, 50.0f, 500.0f, 5000.0f };

		float m_nearPlane = -5.0f;
		float m_farPlane = 5.0f;

		Vector3f m_lightDirection = Vector3f(0.0f, 1.0f, 0.0f);
		Vector3f m_lightOrigin = Vector3f::Zero;

	};

}
