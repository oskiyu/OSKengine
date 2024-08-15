#include "Gjk.h"

OSK::COLLISION::MinkowskiSupport OSK::COLLISION::GetMinkowskiSupport(const IGjkCollider& first, const IGjkCollider& second, const Vector3f& direction) {
	const auto& firstSupport = first.GetSupport(direction);
	const auto& secondSupport = second.GetSupport(-direction);
	
	MinkowskiSupport output{};
	output.point = firstSupport.point - secondSupport.point;
	output.originalVertexIdA = firstSupport.originalVertexId;
	output.originalVertexIdB = secondSupport.originalVertexId;

	return output;
}

OSK::DynamicArray<OSK::COLLISION::MinkowskiSupport> OSK::COLLISION::GetAllMinkowskiSupports(const IGjkCollider& first, const IGjkCollider& second, const Vector3f& direction, float epsilon) {
	const auto& firstSupports = first.GetAllSupports(direction, epsilon);
	const auto& secondSupports = second.GetAllSupports(-direction, epsilon);

	DynamicArray<MinkowskiSupport> output = DynamicArray<MinkowskiSupport>::CreateReserved(firstSupports.GetSize() * secondSupports.GetSize());

	for (UIndex64 a = 0; a < firstSupports.GetSize(); a++) {
		for (UIndex64 b = a + 1; b < firstSupports.GetSize(); b++) {
			const auto& supportA = firstSupports[a];
			const auto& supportB = firstSupports[b];

			for (const auto& s : output) {
				if (s.originalVertexIdA == supportA.originalVertexId && s.originalVertexIdB == supportB.originalVertexId) {
					continue;
				}
			}

			MinkowskiSupport support{};
			support.point = supportA.point - supportB.point;
			support.originalVertexIdA = supportA.originalVertexId;
			support.originalVertexIdB = supportB.originalVertexId;

			output.Insert(support);
		}
	}

	return output;
}
