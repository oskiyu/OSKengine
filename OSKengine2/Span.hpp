// OSKengine by oskiyu
// Copyright (c) 2019-2022 oskiyu. All rights reserved.
#pragma once

#include "OSKmacros.h"

namespace OSK {

	template <typename T>
	class Span {

	public:

		explicit Span(const T* data) {
			this->data = data;
			this->count = sizeof(data) / sizeof(T);
		}

		explicit Span(const T* data, TSize count) {
			this->data = data;
			this->count = count;
		}

		explicit Span(const T* data, TSize start, TSize end) {
			this->data = data;
			this->start = start;
			this->count = end - start;
		}

	private:

		T* data = nullptr;
		TSize start = 0;
		TSize count = 0;

	};

}
