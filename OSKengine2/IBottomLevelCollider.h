// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"
#include "Vector3.hpp"
#include "IRayCollider.h"
#include "Transform3D.h"

namespace OSK::COLLISION {

	class Collider;


	/// @brief Representa un �rea de colisi�n no muy ajustada
	/// que permite descartar colisiones de una manera m�s r�pida.
	class OSKAPI_CALL IBottomLevelCollider : public IRayCollider {

	public:

		virtual ~IBottomLevelCollider() = default;
		OSK_DEFINE_AS(IBottomLevelCollider);


		void SetTransformOffset(const ECS::Transform3D& offset);
		const ECS::Transform3D& GetTransformOffset() const;


		/// @brief Comprueba si este �rea de colisi�n est� en contacto
		/// con otra.
		/// @param other Otra �rea de colisi�n de nivel bajo.
		/// @param thisOffset Posici�n de este collider.
		/// @param otherOffset Posici�n del segundo collider.
		virtual bool IsColliding(const IBottomLevelCollider& other,
			const ECS::Transform3D& transformA, const ECS::Transform3D& transformB) const = 0;

		virtual bool ContainsPoint(const Vector3f& point) const = 0;

	protected:

	private:

		ECS::Transform3D transformOffset;

	};

}