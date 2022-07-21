#pragma once

#include "RenderTarget.h"
#include "IGpuUniformBuffer.h"
#include "MaterialInstance.h"
#include "Vector4.hpp"

namespace OSK::GRAPHICS {

	class DirectionalLight;

	/// <summary>
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
	/// </summary>
	class OSKAPI_CALL ShadowMap {

	public:

		/// <summary>
		/// Inicializa el shadow map.
		/// </summary>
		/// <param name="imageSize">Resolución de la imagen del shadow map.
		/// Cuanto más alta, mayor calidad de sombras.</param>
		void Create(const Vector2ui& imageSize);

		/// <summary>
		/// Establece la luz direccional que proyectará las sobras
		/// sobre este mapa de sombras.
		/// </summary>
		/// <param name="dirLight">Luz direccional que proyectará sombras.</param>
		/// 
		/// @pre Debe llamarse después de inicializar el mapa de sombras (ShadowMap::Create).
		void SetDirectionalLight(const DirectionalLight& dirLight);


		/// <summary>
		/// Establece el área de la escena que será renderizada al
		/// mapa de sombras.
		/// 
		/// Cuanto mayor sea más área será sombreada, pero las sombras
		/// tendrán una menor resolución
		/// </summary>
		/// <param name="area">Área de la escena sombreada, en metros.</param>
		/// 
		/// @note Por defecto, 100, 100.
		void SetLightArea(const Vector2f& area);

		/// <summary>
		/// Establece el near plane del renderizado.
		/// </summary>
		/// <param name="nearPlane">Distancia mínima al origen
		/// de la luz para que sean renderizados.</param>
		/// 
		/// @note Por defecto, -5.0f.
		void SetNearPlane(float nearPlane);

		/// <summary>
		/// Establece el far plane del renderizado.
		/// </summary>
		/// <param name="farPlane">Distancia máxima al origen
		/// de la luz para que sean renderizados.</param>
		/// 
		/// @note Por defecto, 5.0f.
		void SetFarPlane(float farPlane);

		/// <summary>
		/// Establece el punto del que se origina la luz direccional.
		/// 
		/// Aunque teóricamente la luz direccional se origina en un punto
		/// infinitamente lejano, se debe usar un punto de origen para
		/// saber qué área de la escena se debe procesar, ya que no disponemos
		/// de una imagen infinitamente grande.
		/// </summary>
		/// 
		/// <param name="origin">Punto desde el que se renderizarán las sombras.</param>
		/// 
		/// @note Debe estar lo suficientemente alto como para que los objetos puedan
		/// proyectar sombras.
		/// @note Por defecto, será el punto (0, 0, 0) - dirLight.direction * 50, generando
		/// sombras para los elementos en el centro de la escena.
		void SetLightOrigin(const Vector3f& origin);

		/// <summary>
		/// Devuelve el punto del que se origina la luz direccional en este instante.
		/// </summary>
		Vector3f GetCurrentLigthOrigin() const;


		/// <summary>
		/// Devuelve la imagen del mapa de sombras.
		/// </summary>
		/// <param name="index">Índice de la imagen en el swapchain.</param>
		/// <returns>Mapa de profundidad.</returns>
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolverá nullptr si no se cumple la precondición.
		GpuImage* GetShadowImage(TSize index) const;

		/// <summary>
		/// Devuelve el render target donde se renderizará el
		/// mapa de sombras.
		/// </summary>
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @warning Devolverá un render target sin inicializar si no se cumple
		/// la precondición.
		RenderTarget* GetShadowsRenderTarget();

		/// <summary>
		/// Devuelve el material usado para la generación de sombras.
		/// </summary>
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolverá nullptr si no se cumple la precondición.
		Material* GetShadowsMaterial() const;

		/// <summary>
		/// Devuelve la instancia del material para la generación de sombras.
		/// </summary>
		/// 
		/// @note Se debe vincular el slot "global" para poder ser usado correctamente.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolverá nullptr si no se cumple la precondición.
		MaterialInstance* GetShadowsMaterialInstance() const;

		/// <summary>
		/// Devuelve el uniform buffer que contiene la matriz de
		/// proyección y vista de la luz que proyecta la sombra.
		/// </summary>
		/// 
		/// @pre Se ha llamado a ShadowMap::Create.
		/// @warning Devuelve nullptr si no se cumple las precondiciones.
		GRAPHICS::IGpuUniformBuffer* GetDirLightMatrixUniformBuffer() const;

	private:

		void UpdateLightMatrixBuffer();

		RenderTarget shadowsTarget;

		Vector2f lightArea = Vector2f(5.f, 5.f);
		float nearPlane = -2.0f;
		float farPlane = 2.0f;

		Vector3f lightDirection = Vector3f(0.0f, 1.0f, 0.0f);
		Vector3f lightOrigin = Vector3f(0.0f);

		Material* shadowsGenMaterial = nullptr;
		UniquePtr<MaterialInstance> shadowsGenMaterialInstance = nullptr;

		UniquePtr<IGpuUniformBuffer> lightUniformBuffer{};

	};

}
