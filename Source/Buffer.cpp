#include "Buffer.h"

using namespace OSK;
using namespace OSK::MEMORY;

Buffer::Buffer(USize64 initialSize)
	: m_data(DynamicArray<TByte>::CreateReserved(initialSize)) {

}

USize64 Buffer::GetSize() const {
	return m_data.GetSize();
}

void Buffer::AddData(const void* data, USize64 size) {
	for (UIndex64 i = 0; i < size; i++) {
		TByte d = std::bit_cast<TByte>(*(static_cast<const TByte*>(data) + i));
		m_data.Insert(d);
	}
}

const void* Buffer::GetRawData() const {
	return m_data.GetData();
}

void* Buffer::GetRawData() {
	return m_data.GetData();
}

const void* Buffer::GetRawData(UIndex64 byteOffset) const {
	return &m_data.At(byteOffset);
}

void* Buffer::GetRawData(UIndex64 byteOffset) {
	return &m_data.At(byteOffset);
}
