// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "Field.h"

using namespace OSK;
using namespace OSK::PERSISTENCE;

#define OSK_DEFINE_FIELD_WRAPPER(type, datatype) \
FieldWrapper<type>::FieldWrapper() : IFieldWrapper(datatype) { } \
FieldWrapper<type>::FieldWrapper(const type & data) : IFieldWrapper(datatype) { \
	this->data.Insert(data); \
} \

OSK_DEFINE_FIELD_WRAPPER(std::string, DataType::STRING);
OSK_DEFINE_FIELD_WRAPPER(int, DataType::INT);
OSK_DEFINE_FIELD_WRAPPER(float, DataType::FLOAT);
