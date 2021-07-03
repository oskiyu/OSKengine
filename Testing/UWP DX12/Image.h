#pragma once

#include "Engine.h"

class Image {

	friend class MemoryAllocator;

public:



private:

	ComPtr<ID3D12Resource> gpuImage;

};
