#pragma once

#include "pch.h"

#include <stdexcept>
#include <string>

using Microsoft::WRL::ComPtr;

bool IsDxSuccess(HRESULT result) {
	return result >= 0;
}

#define OSK_ASSERT(condition, msg) if (!condition) throw std::runtime_error(std::string(msg));
#define OSK_ASSERT2(condition, msg, code) if (!condition) throw std::runtime_error(std::string(msg) + " Code: " + std::to_string(code) + ".");

#define SET_DX12_OBJECT_NAME(object) object->SetName(L#object);