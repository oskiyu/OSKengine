#pragma once

namespace UWP_DX12
{
	// Búfer de constantes usado para enviar matrices MVP al sombreador de vértices.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Se usa para enviar datos de vértice al sombreador de vértices.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};
}