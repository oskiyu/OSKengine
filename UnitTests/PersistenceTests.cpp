// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(VectorTests) {

public:

	/// @brief Dos esferas de radio 1 separadas por 0.9m.
	TEST_METHOD(BitCastTest) {
		float value = 1.2345f;
		const char* bits = std::bit_cast<const char*>(bits);
		const float fvalue = std::bit_cast<float>(()bits);

		Assert::AreEqual(value, fvalue);
	}

};
