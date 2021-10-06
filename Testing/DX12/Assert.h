#pragma once

#include <string>
#include <stdexcept>

#define OSK_ASSERT(condition, msg) if (!condition) throw std::runtime_error(std::string(msg));
#define OSK_ASSERT2(condition, msg, code) if (!condition) throw std::runtime_error(std::string(msg) + " Code: " + std::to_string(code) + ".");
