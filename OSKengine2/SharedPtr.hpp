#ifndef OSK_SHARED_PTR
#define OSK_SHARED_PTR

#include <functional>

#include "OSKmacros.h"

/// <summary>
/// Contiene un puntero al que pueden hacer referencia varios SharedPtr.
/// Si el último SharedPtr que hace referencia a un puntero es desstruido, el puntero será eliminado.
/// 
/// @note La propiedad del puntero está compartida.
/// </summary>
/// <typeparam name="T">Tipo del puntero almacenado.</typeparam>
template <typename T> 
class SharedPtr {

public:

	constexpr SharedPtr() noexcept : m_instanceCount(new USize64(0uL)) { }

	/// El puntero debe haberse creado con new.
	constexpr SharedPtr(T* data) noexcept : m_pointer(data), m_instanceCount(new USize64(1uL)) { }

	SharedPtr(const SharedPtr& other) : m_pointer(other.m_pointer), m_instanceCount(other.m_instanceCount) {
		if (m_pointer)
			(*m_instanceCount)++;
	}

	~SharedPtr() {
		destructor();
	}

	SharedPtr& operator=(const SharedPtr& other) {
		destructor();

		m_pointer = other.m_pointer;
		m_instanceCount = other.m_instanceCount;
		
		if (m_pointer)
			(*m_instanceCount)++;

		return *this;
	}

	SharedPtr(SharedPtr&& other) noexcept : m_pointer(other.m_pointer), m_instanceCount(other.m_instanceCount) {
		other.m_pointer = nullptr;
		other.m_instanceCount = new USize64(0uL);
	}

	SharedPtr& operator=(SharedPtr&& other) noexcept {
		destructor();

		m_pointer = other.m_pointer;
		m_instanceCount = other.m_instanceCount;

		other.m_pointer = nullptr;
		other.m_instanceCount = new USize64(0uL);

		return *this;
	}

	/// <summary>
	/// Devuelve el número de SharedPtr que comparten el mismo puntero que este.
	/// </summary>
	/// <returns>Número de instancias.</returns>
	constexpr USize64 GetInstanceCount() const noexcept {
		return *m_instanceCount;
	}

	template <typename Self>
	auto&& GetPointer(this Self&& self) {
		return ::std::forward<Self>(self).operator->();
	}

	template <typename Self>
	auto&& Get(this Self&& self) {
		return ::std::forward<Self>(self).operator*();
	}

	template <typename Self>
	auto&& operator*(this Self&& self) {
		return *self.m_pointer;
	}
	template <typename Self>
	auto&& operator->(this Self&& self) {
		return self.m_pointer;
	}

	constexpr bool HasValue() const {
		return m_pointer != nullptr;
	}

	constexpr bool operator==(const SharedPtr&) const = default;

private:

	void destructor() {
		if (!m_instanceCount)
			return;

		(*m_instanceCount)--;

		if (*m_instanceCount == 0 && m_pointer) {
			delete m_pointer;
			delete m_instanceCount;
		}

		m_instanceCount = nullptr;
		m_pointer = nullptr;
	}

	/// Puntero compartido.
	T* m_pointer = nullptr;

	/// Número de SharedPtr con este puntero.
	USize64* m_instanceCount = nullptr;

};

#endif
