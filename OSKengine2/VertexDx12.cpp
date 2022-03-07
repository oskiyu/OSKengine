#include "VertexDx12.h"

#include "Vertex.h"

D3D12_INPUT_LAYOUT_DESC OSK::GRAPHICS::GetInputLayoutDescDx12_Vertex3D() {
	
	const static D3D12_INPUT_ELEMENT_DESC descs[] = {
		// SemanticName; SemanticIndex; Format; InputSlot; AlignedByteOffset; InputSlotClass; InstanceDataStepRate;

		{ "position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex3D, color), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "texCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex3D, texCoords), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayout{};
	inputLayout.NumElements = _countof(descs);
	inputLayout.pInputElementDescs = descs;

	return inputLayout;
}
