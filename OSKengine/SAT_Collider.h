#pragma once

#include "OSKtypes.h"

//#include "SAT_Face.h"
#include "SAT_Projection.hpp"
#include "SAT_CollisionInfo.h"

#include "DynamicArray.h"

#include <vector>
#include "Transform.h"

namespace OSK::Collision {

	typedef std::vector<VectorElementPtr_t> SAT_Face;

	class SAT_Collider {

	public:

		~SAT_Collider();

		void OptimizeFaces();

		void AddFace(const Vector3f points[], const uint32_t& size);

		bool Intersects(const SAT_Collider& other) const;

		SAT_CollisionInfo GetCollisionInfo(const SAT_Collider& other) const;

		void TransformPoints();

		//Transform de este OBB.
		Transform BoxTransform{};

		static SAT_Collider CreateOBB();

		std::vector<Vector3f> GetPoints() const;

		inline std::vector<Vector3f> GetAxes() const {
			std::vector<Vector3f> axes = {};
			axes.reserve(Faces.size());

			for (const auto& i : Faces) {
				axes.push_back(GetAxisFromFace(i));
			}

			return axes;
		}

	private:
		
		inline const bool isAllStatic() const {
			return staticRotation && staticPosition && staticScale;
		}

		Vector3f GetAxisFromFace(const SAT_Face& face) const;

		SAT_Projection ProjectToAxis(const Vector3f& axis) const;

		std::vector<Vector3f> Points = {};
		std::vector<Vector3f> TransformedPoints = {};

		std::vector<SAT_Face> Faces;

		bool staticRotation = false;
		bool staticPosition = false;
		bool staticScale = false;

	};

}