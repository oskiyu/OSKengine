#pragma once

#include "ApiCall.h"

#include "SphereCollider.h"
#include "BroadColliderHolder.h"

#include "Uuid.h"


namespace OSK::GRAPHICS {

	/// @brief Identificador �nico para una malla de GPU.
	using GpuMeshUuid = BaseUuid<class GpuMeshUuidTag>;

	/// @brief Malla que engloba una serie de primitivas que comparten
	/// un mismo material.
	/// 
	/// @note Los buffers con v�rtices e �ndices son propiedad del modelo 3D.
	/// 
	/// @note Un mesh �nicamente almacena los �ndices que le corresponden,
	/// representados por el ID del primer �ndice y el n�mero de �ndices.
	class OSKAPI_CALL GpuMesh3D {

	public:

		/// @brief Inicializa la malla.
		/// @param firstIndex �ndice del primer �ndice en el buffer de �ndices del modelo.
		/// @param numIndices N�mero de �ndices de la malla.
		/// @param sphereCenter Centro de la esfera que contiene la malla.
		/// @param materialIndex �ndice del material (con respecto a los materiales del modelo).
		GpuMesh3D(
			GpuMeshUuid uuid,
			UIndex32 firstIndex,
			USize32 numIndices,
			const Vector3f& sphereCenter,
			UIndex64 materialIndex);


		/// @return �ndice del primer �ndice en el buffer de
		/// �ndices del modelo.
		UIndex32 GetFirstIndexIdx() const;

		/// @return N�mero de �ndices que componen la malla.
		USize32 GetNumIndices() const;


		/// @return Esfera que contiene todos los v�rtices de la malla.
		const COLLISION::SphereCollider& GetBounds() const;

		/// @return Esfera que contiene todos los v�rtices de la malla.
		COLLISION::SphereCollider& GetBounds();

		/// @return Centro de la esfera que contiene todos los v�rtices de la malla.
		const Vector3f& GetSphereCenter() const;

		/// @brief Establece el radio de la esfera que debe contener todos los v�rtices de la malla.
		/// @param radius Radio de la esfera.
		/// @pre El radio debe ser tal que la esfera contenga todos los v�rtices de la malla.
		void SetBoundingSphereRadius(float radius);


		/// @return Identificador �nico de este mesh.
		GpuMeshUuid GetUuid() const;

		UIndex64 GetMaterialIndex() const;

	private:

		GpuMeshUuid m_uuid = GpuMeshUuid::CreateEmpty();

		UIndex32 m_firstIndex = 0;
		USize32 m_numIndices = 0;

		Vector3f m_sphereCenter = Vector3f::Zero;
		UniquePtr<COLLISION::BroadColliderHolder> m_sphere{};

		UIndex64 m_materialIndex = 0;

	};

}

OSK_DEFINE_UUID_HASH(OSK::GRAPHICS::GpuMeshUuid)
