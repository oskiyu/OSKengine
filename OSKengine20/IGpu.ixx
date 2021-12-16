export module OSKengine.Graphics.IGpu;

export namespace OSK {
		
	/// <summary>
	/// Interfaz de una tarjeta gr�fica.
	/// Esta interfaz no implementa ning�n c�digo, se debe usar una instancia de
	/// GpuVulkan o GpuDx12.
	/// </summary>
	class IGpu {

	public:

		virtual ~IGpu() {

		}

		/// <summary>
		/// Castea la clase al tipo dado.
		/// Este tipo debe ser una implementaci�n de esta interfaz.
		/// </summary>
		template <typename T> T* As() {
			return (T*)this;
		}

	protected:

		IGpu() { }

	private:

	};

}
