#pragma once

#include <OSKengine/Component.h>

#include <OSKengine/Spline3D.h>
#include <OSKengine/AssetRef.h>
#include <OSKengine/PreBuiltSpline3D.h>

struct CircuitComponent {

	OSK_COMPONENT("CircuitComponent");

	OSK::ASSETS::AssetRef<OSK::ASSETS::PreBuiltSpline3D> trazadaIdeal;

};
