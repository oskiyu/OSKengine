#ifndef OSK_UNIQUE_PTR
#define OSK_UNIQUE_PTR

namespace OSK {

	/// <summary>
	/// Es dueño de un puntero.
	/// El puntero original es eliminado al destruirse el UniquePtr.
	/// </summary>
	/// <typeparam name="T">Tipo del puntero.</typeparam>
	template <typename T> class UniquePtr {

#define OSK_STD_UP_DELETE(ptr) if (ptr) delete(ptr);

	public:

		/// <summary>
		/// Crea un UniquePtr vacío.
		/// </summary>
		UniquePtr() : pointer(nullptr) {

		}

		UniquePtr(T* data) : pointer(data) {

		}

		/// <summary>
		/// Crea un UniquePtr que será dueño de 'data'.
		/// </summary>
		/// <param name="data">Puntero original.</param>
		void operator=(T* data) {
			OSK_STD_UP_DELETE(pointer)
				pointer = data;
		}

		/// <summary>
		/// Destruye el puntero, eliminándolo.
		/// <summary/>
		~UniquePtr() {
			OSK_STD_UP_DELETE(pointer)
		}

		inline UniquePtr(const UniquePtr&) = delete;
		inline UniquePtr& operator=(const UniquePtr&) = delete;

		/// <summary>
		/// Este puntero será dueño del puntero de other.
		/// </summary>
		/// <param name="other">Otro puntero.</param>
		UniquePtr(UniquePtr&& other) {
			other.Swap(*this);
		}

		/// <summary>
		/// Este puntero será dueño del puntero de other.
		/// </summary>
		/// <param name="other">Otro puntero.</param>
		/// <returns>Self.</returns>
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			other.Swap(*this);

			return *this;
		}

		/// <summary>
		/// 
		/// </summary>
		T* Release() {
			T* output = pointer;
			pointer = nullptr;

			return output;
		}

		/// <summary>
		/// Cambia el puntero de este UniquePtr (eliminando el anterior).
		/// </summary>
		/// <param name="newValue">Nuevo valor.</param>
		void SetNewValue(T* newValue) {
			OSK_STD_UP_DELETE(pointer)

				pointer = newValue;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* GetPointer() const {
			return pointer;
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& GetValue() {
			return *pointer;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* operator->() const {
			return GetPointer();
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& operator*() {
			return GetValue();
		}

		/// <summary>
		/// Devuelve true si el puntero no es null.
		/// </summary>
		/// <returns>Estado del puntero.</returns>
		bool HasValue() const {
			return pointer != nullptr;
		}

		/// <summary>
		/// Elimina el puntero.
		/// </summary>
		inline void Delete() {
			OSK_STD_UP_DELETE(pointer);
			pointer = nullptr;
		}

		void Swap(UniquePtr<T>& other) {
			T* temp = pointer;
			pointer = other.pointer;
			other.pointer = temp;
		}

		bool operator==(const UniquePtr& other) const {
			return pointer == other.pointer;
		}
		bool operator!=(const UniquePtr& other) const {
			return pointer != other.pointer;
		}

	private:

		/// <summary>
		/// Puntero.
		/// </summary>
		T* pointer = nullptr;
#undef OSK_STD_UP_DELETE
	};

	/// <summary>
	/// Es dueño de un puntero.
	/// El puntero original es eliminado al destruirse el UniquePtr.
	/// </summary>
	/// <typeparam name="T">Tipo del puntero.</typeparam>
	template <typename T> class UniquePtr<T[]> {

#define OSK_STD_UP_DELETE(ptr) if (ptr) delete[](ptr);

	public:

		/// <summary>
		/// Crea un UniquePtr vacío.
		/// </summary>
		UniquePtr() : pointer(nullptr) {

		}

		explicit UniquePtr(T* data) : pointer(data) {

		}

		/// <summary>
		/// Crea un UniquePtr que será dueño de 'data'.
		/// </summary>
		/// <param name="data">Puntero original.</param>
		void operator=(T* data) {
			OSK_STD_UP_DELETE(pointer)
				pointer = data;
		}

		/// <summary>
		/// Destruye el puntero, eliminándolo.
		/// <summary/>
		~UniquePtr() {
			OSK_STD_UP_DELETE(pointer)
		}

		inline UniquePtr(const UniquePtr&) = delete;
		inline UniquePtr& operator=(const UniquePtr&) = delete;

		/// <summary>
		/// Este puntero será dueño del puntero de other.
		/// </summary>
		/// <param name="other">Otro puntero.</param>
		UniquePtr(UniquePtr&& other) {
			other.Swap(*this);

			return *this;
		}

		/// <summary>
		/// Este puntero será dueño del puntero de other.
		/// </summary>
		/// <param name="other">Otro puntero.</param>
		/// <returns>Self.</returns>
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			other.Swap(*this);

			return *this;
		}

		/// <summary>
		/// 
		/// </summary>
		T* Release() {
			T* output = pointer;
			pointer = nullptr;

			return output;
		}

		/// <summary>
		/// Cambia el puntero de este UniquePtr (eliminando el anterior).
		/// </summary>
		/// <param name="newValue">Nuevo valor.</param>
		void SetNewValue(T* newValue) {
			OSK_STD_UP_DELETE(pointer)

				pointer = newValue;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* GetPointer() const {
			return pointer;
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& GetValue() {
			return *pointer;
		}

		/// <summary>
		/// Devuelve el puntero nativo.
		/// </summary>
		/// <returns>Puntero.</returns>
		T* operator->() const {
			return GetPointer();
		}

		/// <summary>
		/// Devuelve el valor apuntado por el puntero.
		/// </summary>
		/// <returns>Valor.</returns>
		T& operator*() {
			return GetValue();
		}

		/// <summary>
		/// Devuelve true si el puntero no es null.
		/// </summary>
		/// <returns>Estado del puntero.</returns>
		bool HasValue() const {
			return pointer != nullptr;
		}

		/// <summary>
		/// Elimina el puntero.
		/// </summary>
		inline void Delete() {
			OSK_STD_UP_DELETE(pointer);
			pointer = nullptr;
		}

		void Swap(UniquePtr<T>& other) {
			T* temp = pointer;
			pointer = other.pointer;
			other.pointer = temp;
		}

	private:

		/// <summary>
		/// Puntero.
		/// </summary>
		T* pointer = nullptr;

	};
}

#undef OSK_STD_UP_DELETE
#endif
