// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "pch.h"
#include "CppUnitTest.h"

#include "../OSKengine2/Vector4.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace OSK;

TEST_CLASS(VectorTests) {

public:

	/// @brief Dos esferas de radio 1 separadas por 0.9m.
	TEST_METHOD(ContainsPointTest) {
		Vector4f rectangle = {
			1.0f, 1.0f, 5.0f, 5.0f
		};
		Vector2f point = { 2.0f, 3.0f };

		const bool result = rectangle.ContainsPoint(point);
		Assert::IsTrue(result);
	}

};
