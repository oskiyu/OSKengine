// OSKengine by oskiyu
// Copyright (c) 2019-2023 oskiyu. All rights reserved.
#include "Field.h"

using namespace OSK;
using namespace OSK::PERSISTENCE;

FieldType SingleFieldWrapper<std::string>::GetFieldType() const {
	return FieldType::STRING;
}

FieldType SingleFieldWrapper<int>::GetFieldType() const {
	return FieldType::INT;
}

FieldType SingleFieldWrapper<float>::GetFieldType() const {
	return FieldType::FLOAT;
}


FieldType ListFieldWrapper<std::string>::GetFieldType() const {
	return FieldType::STRING;
}

FieldType ListFieldWrapper<int>::GetFieldType() const {
	return FieldType::INT;
}

FieldType ListFieldWrapper<float>::GetFieldType() const {
	return FieldType::FLOAT;
}
