#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Content\Sample3DSceneRenderer.h"

// Representa contenido Direct3D en la pantalla.
namespace UWP_DX12
{
	class UWP_DX12Main
	{
	public:
		UWP_DX12Main();
		void CreateRenderers(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Update();
		bool Render();

		void OnWindowSizeChanged();
		void OnSuspending();
		void OnResuming();
		void OnDeviceRemoved();

	private:
		// TODO: Sustituir con sus propios representadores de contenido.
		std::unique_ptr<Sample3DSceneRenderer> m_sceneRenderer;

		// Temporizador de bucle de representación.
		DX::StepTimer m_timer;
	};
}