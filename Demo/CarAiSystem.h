#pragma once

#include <OSKengine/IPureSystem.h>

#include <unordered_map>
#include <random>

#include <OSKengine/DynamicArray.hpp>

#include <OSKengine/Spline3D.h>

class CarAiSystem : public OSK::ECS::IPureSystem {

public:

	OSK_SYSTEM("CarAiSystem");

	void OnCreate() override;
	void OnTick(TDeltaTime) override;

	void OnObjectAdded(OSK::ECS::GameObjectIndex car);

	void SetCircuit(OSK::ECS::GameObjectIndex circuit);

private:

	struct Rng {
		std::uniform_int_distribution<std::mt19937::result_type> distribution;
		std::mt19937 engine;

		USize32 Generate() {
			return distribution(engine);
		}
	};

	struct SplineInterval {
		float a;
		float b;
		float c;
		float d;

		float Calculate(float t) const {
			return a * t * t * t + b * t * t + c * t + d;
		}
	};

	SplineInterval CalculateIntervalX(const OSK::Spline3D& spline, UIndex64 a, UIndex64 b, UIndex64 c, UIndex64 d) const;
	SplineInterval CalculateIntervalY(const OSK::Spline3D& spline, UIndex64 a, UIndex64 b, UIndex64 c, UIndex64 d) const;
	SplineInterval CalculateIntervalZ(const OSK::Spline3D& spline, UIndex64 a, UIndex64 b, UIndex64 c, UIndex64 d) const;

	SplineInterval CalculateInterval1D(float a, float b, float c, float d) const;
	SplineInterval CalculateIntervalWithDerivate1D(float firstDerivate, float b, float c, float d) const;
	SplineInterval CalculateIntervalInternal1D(float firstDerivate, float secondDerivate, float b, float c) const;

	OSK::DynamicArray<SplineInterval> xIntervals;
	OSK::DynamicArray<SplineInterval> yIntervals;
	OSK::DynamicArray<SplineInterval> zIntervals;

	OSK::ECS::GameObjectIndex m_circuit = OSK::ECS::EMPTY_GAME_OBJECT;

	std::unordered_map<OSK::ECS::GameObjectIndex, Rng> rngs;

};
