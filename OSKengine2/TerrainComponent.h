#pragma once

#include "Component.h"
#include "UniquePtr.hpp"
#include "GpuBuffer.h"
#include "Vector2.hpp"
#include "MaterialInstance.h"

namespace OSK::ECS {

	/// <summary>
	/// Componente que se puede usar para renderizar un terreno.
	/// El terreno debe haber sido antes generado.
	/// 
	/// A la hora de generarse, se generar� un terreno con dimensiones { 1, 0, 1 }:
	/// los v�rtices no tendr�n altura ninguna.
	/// Las coordenadas de texturas se asignar�n para que una textura est� entera
	/// dentro del terreno.
	/// </summary>
	/// 
	/// @note Si se le quiere dar relieve al terreno, se debe hacer a trav�s de shaders.
	/// 
	/// @warning Se debe establecer la instancia del material (TerrainComponent::SetMaterialInstance()).
	/// 
	/// @deprecated No mantenido.
	struct OSKAPI_CALL TerrainComponent {

	public:

		OSK_COMPONENT("OSK::TerrainComponent");

		/// <summary>
		/// Genera una malla de X * Y v�rtices.
		/// </summary>
		void Generate(const Vector2ui& resolution);

		/// <summary>
		/// Establece la instancia del material para el renderizado.
		/// </summary>
		void SetMaterialInstance(OwnedPtr<GRAPHICS::MaterialInstance> materialInstance);

		const GRAPHICS::GpuBuffer* GetVertexBuffer() const;
		const GRAPHICS::GpuBuffer* GetIndexBuffer() const;

		GRAPHICS::MaterialInstance* GetMaterialInstance() const;

		USize32 GetNumIndices() const;

	private:

		UniquePtr<GRAPHICS::GpuBuffer> vertexBuffer;
		UniquePtr<GRAPHICS::GpuBuffer> indexBuffer;

		UniquePtr<GRAPHICS::MaterialInstance> materialInstance;

		UIndex32 numIndices = 0;

	};

}
