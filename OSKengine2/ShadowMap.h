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
		/// <param name="imageSize">Resoluci�n de la imagen del shadow map.
		/// Cuanto m�s alta, mayor calidad de sombras.</param>
		void Create(const Vector2ui& imageSize);

		/// <summary>
		/// Establece la luz direccional que proyectar� las sobras
		/// sobre este mapa de sombras.
		/// </summary>
		/// <param name="dirLight">Luz direccional que proyectar� sombras.</param>
		/// 
		/// @pre Debe llamarse despu�s de inicializar el mapa de sombras (ShadowMap::Create).
		void SetDirectionalLight(const DirectionalLight& dirLight);


		/// <summary>
		/// Establece el �rea de la escena que ser� renderizada al
		/// mapa de sombras.
		/// 
		/// Cuanto mayor sea m�s �rea ser� sombreada, pero las sombras
		/// tendr�n una menor resoluci�n
		/// </summary>
		/// <param name="area">�rea de la escena sombreada, en metros.</param>
		/// 
		/// @note Por defecto, 100, 100.
		void SetLightArea(const Vector2f& area);

		/// <summary>
		/// Establece el near plane del renderizado.
		/// </summary>
		/// <param name="nearPlane">Distancia m�nima al origen
		/// de la luz para que sean renderizados.</param>
		/// 
		/// @note Por defecto, -5.0f.
		void SetNearPlane(float nearPlane);

		/// <summary>
		/// Establece el far plane del renderizado.
		/// </summary>
		/// <param name="farPlane">Distancia m�xima al origen
		/// de la luz para que sean renderizados.</param>
		/// 
		/// @note Por defecto, 5.0f.
		void SetFarPlane(float farPlane);

		/// <summary>
		/// Establece el punto del que se origina la luz direccional.
		/// 
		/// Aunque te�ricamente la luz direccional se origina en un punto
		/// infinitamente lejano, se debe usar un punto de origen para
		/// saber qu� �rea de la escena se debe procesar, ya que no disponemos
		/// de una imagen infinitamente grande.
		/// </summary>
		/// 
		/// <param name="origin">Punto desde el que se renderizar�n las sombras.</param>
		/// 
		/// @note Debe estar lo suficientemente alto como para que los objetos puedan
		/// proyectar sombras.
		/// @note Por defecto, ser� el punto (0, 0, 0) - dirLight.direction * 50, generando
		/// sombras para los elementos en el centro de la escena.
		void SetLightOrigin(const Vector3f& origin);

		/// <summary>
		/// Devuelve el punto del que se origina la luz direccional en este instante.
		/// </summary>
		Vector3f GetCurrentLigthOrigin() const;


		/// <summary>
		/// Devuelve la imagen del mapa de sombras.
		/// </summary>
		/// <param name="index">�ndice de la imagen en el swapchain.</param>
		/// <returns>Mapa de profundidad.</returns>
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolver� nullptr si no se cumple la precondici�n.
		GpuImage* GetShadowImage(TSize index) const;

		/// <summary>
		/// Devuelve el render target donde se renderizar� el
		/// mapa de sombras.
		/// </summary>
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @warning Devolver� un render target sin inicializar si no se cumple
		/// la precondici�n.
		RenderTarget* GetShadowsRenderTarget();

		/// <summary>
		/// Devuelve el material usado para la generaci�n de sombras.
		/// </summary>
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolver� nullptr si no se cumple la precondici�n.
		Material* GetShadowsMaterial() const;

		/// <summary>
		/// Devuelve la instancia del material para la generaci�n de sombras.
		/// </summary>
		/// 
		/// @note Se debe vincular el slot "global" para poder ser usado correctamente.
		/// 
		/// @pre Se debe haber inicializado el ShadowMap con ShadowMap::Create.
		/// @note Devolver� nullptr si no se cumple la precondici�n.
		MaterialInstance* GetShadowsMaterialInstance() const;

		/// <summary>
		/// Devuelve el uniform buffer que contiene la matriz de
		/// proyecci�n y vista de la luz que proyecta la sombra.
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
