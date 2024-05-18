#include "BinaryBlock.h"

#include "InvalidArgumentException.h"
#include "InvalidObjectStateException.h"

using namespace OSK;
using namespace OSK::PERSISTENCE;


#pragma region BinaryBlockReader

BinaryBlockReader::BinaryBlockReader(std::span<const TByte> data) : m_data(data), m_originalIndex(0) {

}

BinaryBlockReader::BinaryBlockReader(std::span<const TByte> data, UIndex64 offset) : m_data(data), m_index(offset), m_originalIndex(offset) {

}


std::string BinaryBlockReader::ReadString() {

	std::string output{};

	while (m_data[m_index] != 0) {
		output.push_back(m_data[m_index]);
		m_index++;

		OSK_ASSERT(m_index < m_data.size(), FinishedBlockReaderException{});
	}

	// Null final.
	m_index++;

	return output;
}

UIndex64 BinaryBlockReader::GetCurrentIndex() const {
	return m_index;
}

UIndex64 BinaryBlockReader::GetOriginalOffset() const {
	return m_originalIndex;
}

#pragma endregion

#pragma region BinaryBlock

BinaryBlock BinaryBlock::Empty() {
	return {};
}

BinaryBlock BinaryBlock::FromData(const DynamicArray<TByte>& data) {
	auto output = BinaryBlock::Empty();

	output.m_data = data;

	return output;
}

void BinaryBlock::WriteString(std::string_view string) {
	for (const char c : string) {
		m_data.Insert(c);
	}
	m_data.Insert(0);
}

USize64 BinaryBlock::GetCurrentSize() const {
	return m_data.GetSize();
}

void BinaryBlock::AppendBlock(const BinaryBlock& block) {
	m_data.InsertAll(block.m_data);
}

std::span<const TByte> BinaryBlock::GetData() const {
	return m_data.GetFullSpan();
}

BinaryBlockReader BinaryBlock::GetReader() const {
	OSK_ASSERT(!m_data.IsEmpty(), InvalidObjectStateException(std::format("El bloque está vacío.")));
	return GetReader_WithOffset(0);
}

BinaryBlockReader BinaryBlock::GetReader_WithOffset(USize64 offset) const {
	OSK_ASSERT(offset < m_data.GetSize(), InvalidArgumentException(std::format("El offset {} es más grande que el tamaño del bloque.", offset)));
	return BinaryBlockReader(m_data.GetFullSpan(), offset);
}

#pragma endregion
