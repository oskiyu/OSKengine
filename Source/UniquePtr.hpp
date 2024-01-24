#ifndef OSK_UNIQUE_PTR
#define OSK_UNIQUE_PTR

#include "OSKmacros.h"
#include <type_traits>

namespace OSK {

	
	/// @brief Estructura que es due�a de un objeto almacenado en heap.
	/// Al destruirse el UniquePtr, tambi�n se destruir� el objeto
	/// y se liberar� la memoria asociada.
	/// 
	/// @tparam T Tipo de dato del objeto.
	/// 
	/// @note Usarse como miembro de una clase deshabilitar� su constructor 
	/// y su operador de copia.
	/// 
	/// @warning Debe incluirse el tipo que se vaya a usar dentro del UniquePtr.
	/// @code #include "T.h" @endcode
	/// De lo contrario, no se llamar� al destructor al eliminarse.
	template <typename T> class UniquePtr {

	public:

		/// @brief Crea un UniquePtr vac�o.
		constexpr UniquePtr() noexcept = default;

		/// @brief Crea un UniquePtr a partir del objeto almacenado en la
		/// direcci�n de memoria dada.
		/// @param data Puntero al objeto almacenado en memoria heap.
		/// @pre @p data debe haber sido creado con el operador new.
		constexpr UniquePtr(T* data) noexcept : m_pointer(data) { }


		/// @brief Destruye el objeto, elimin�ndolo.
		/// Si no ten�a ning�n objeto, no ocurre nada.
		~UniquePtr() {
			if (m_pointer)
				delete m_pointer;
		}

		UniquePtr(const UniquePtr&) noexcept = delete;
		UniquePtr& operator=(const UniquePtr&) noexcept = delete;

		/// @brief Crea un UniquePtr que pasa a poseer el objeto de otro UniquePtr.
		/// El otro UniquePtr se convierte en un UniquePtr vac�o.
		/// @param other El otro puntero.
		/// @post Este UniquePtr pasa a poseer el objeto de @p other.
		/// @post El UniquePtr @p other pasa a estar vac�o.
		/// @note Si @p other est� vac�o, entonces este UniquePtr pasa a estar vac�o.
		UniquePtr(UniquePtr&& other) noexcept : m_pointer(other.Release()) {}
				
		/// @brief Este puntero ser� due�o del puntero de other.
		/// @param other Otro puntero.
		/// @post @p other queda vac�o.
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			Delete();
			m_pointer = other.Release();
			return *this;
		}
		
		/// @brief Devuelve el puntero, y despu�s vac�a
		/// el UniquePtr.
		/// @return Puntero que pose�a.
		/// @post El puntero almacenado pasa a ser nulo.
		T* Release() {
			T* output = m_pointer;
			m_pointer = nullptr;

			return output;
		}

		template <typename Self>
		auto&& GetPointer(this Self&& self) {
			return ::std::forward<Self>(self).operator->();
		}

		template <typename Self>
		auto&& GetValue(this Self&& self) {
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


		constexpr bool HasValue() const { return m_pointer != nullptr; }

		void Delete() {
			static_assert(sizeof(T) > 0, "El tipo de dato no est� definido.");
			if (m_pointer)
				delete m_pointer;

			m_pointer = nullptr;
		}

		
		/// @brief Intercambia el objeto de este UniquePtr por el
		/// objeto del UniquePtr @p other.
		/// @param other Otro UniquePtr, con el que se va a intercambiar
		/// el objeto.
		/// @post Este UniquePtr almacena (y es due�o) del objeto que antes ten�a @p other.
		/// @post El UniquePtr @p other almacena (y es due�o) del objeto que antes ten�a este UniquePtr.
		void Swap(UniquePtr<T>& other) { std::swap(m_pointer, other.m_pointer); }

		constexpr bool operator==(const UniquePtr&) const noexcept = default;

	private:

		/// Puntero.
		T* m_pointer = nullptr;

	};

}

#endif
