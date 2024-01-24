// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "ITopLevelCollider.h"

#include "Serializer.h"


namespace OSK::COLLISION {

	/// @brief Un área de colisión de alto nivel representado
	/// por una caja tridimensional.
	/// 
	/// Esta caja está alineada con los ejes X, Y y Z; y no puede
	/// ser rotada.
	/// 
	/// Por defecto, tiene tamaño ("radio") 0.5.
	class OSKAPI_CALL AxisAlignedBoundingBox : public ITopLevelCollider {

	public:

		template <typename T>
		friend nlohmann::json PERSISTENCE::SerializeJson<T>(const T& data);

		template <typename T>
		friend T PERSISTENCE::DeserializeJson<T>(const nlohmann::json& data);

	public:
		
		/// @brief Caja con tamaño 1.
		AxisAlignedBoundingBox() = default;
		/// @param size Radio (distancia desde un lado hasta el otro).
		explicit AxisAlignedBoundingBox(const Vector3f& size);

		OwnedPtr<ITopLevelCollider> CreateCopy() const override;


		/// @param size Radio total (distancia desde un lado hasta el otro).
		void SetSize(const Vector3f& size);

		/// @return Tamaño total (la mitad hacia cada dirección
		/// desde el centro).
		const Vector3f& GetSize() const;

		RayCastResult CastRay(const Ray& ray) const override;
		bool ContainsPoint(const Vector3f& point) const override;

		bool IsColliding(const ITopLevelCollider& other) const override;

		bool IsBehindPlane(Plane plane) const override;

		/// @return Esquina mínima: posición del vértice con las
		/// coordenadas más pequeñas.
		Vector3f GetMin() const;

		/// @return Esquina máxima: posición del vértice con las
		/// coordenadas más grandes.
		Vector3f GetMax() const;

	private:

		Vector3f m_size = Vector3f(1.0f);

	};

	using AabbCollider = AxisAlignedBoundingBox;

}

namespace OSK::PERSISTENCE {

	template <>
	nlohmann::json SerializeJson<OSK::COLLISION::AxisAlignedBoundingBox>(const OSK::COLLISION::AxisAlignedBoundingBox& data);

	template <>
	OSK::COLLISION::AxisAlignedBoundingBox DeserializeJson<OSK::COLLISION::AxisAlignedBoundingBox>(const nlohmann::json& json);

}
