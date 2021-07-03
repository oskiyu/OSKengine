#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"

namespace UWP_DX12
{
	// Este representador de ejemplo crea una instancia de una canalización de representación básica.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~Sample3DSceneRenderer();
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void Update(DX::StepTimer const& timer);
		bool Render();
		void SaveState();

		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }

	private:
		void LoadState();
		void Rotate(float radians);

	private:
		// Los búferes de constantes deben tener una alineación de 256 bytes.
		static const UINT c_alignedConstantBufferSize = (sizeof(ModelViewProjectionConstantBuffer) + 255) & ~255;

		// Puntero almacenado en caché para los recursos del dispositivo.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Recursos de Direct3D para la geometría de cubo.
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	m_commandList;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>		m_cbvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource>				m_constantBuffer;
		ModelViewProjectionConstantBuffer					m_constantBufferData;
		UINT8*												m_mappedConstantBuffer;
		UINT												m_cbvDescriptorSize;
		D3D12_RECT											m_scissorRect;
		std::vector<byte>									m_vertexShader;
		std::vector<byte>									m_pixelShader;
		D3D12_VERTEX_BUFFER_VIEW							m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW								m_indexBufferView;

		// Variables usadas con el bucle de representación.
		bool	m_loadingComplete;
		float	m_radiansPerSecond;
		float	m_angle;
		bool	m_tracking;
	};
}

