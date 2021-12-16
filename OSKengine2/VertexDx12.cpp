#include "VertexDx12.h"

D3D12_INPUT_LAYOUT_DESC OSK::GetInputLayoutDescDx12_Vertex3D() {
	// SemanticName; SemanticIndex; Format; InputSlot; AlignedByteOffset; InputSlotClass; InstanceDataStepRate;
	D3D12_INPUT_ELEMENT_DESC descs[] = {
		{"position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * 4, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"texCoords", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 3 * 4 + 4 * 4, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_INPUT_LAYOUT_DESC inputLayout{};
	inputLayout.NumElements = 4;
	inputLayout.pInputElementDescs = descs;

	return inputLayout;
}