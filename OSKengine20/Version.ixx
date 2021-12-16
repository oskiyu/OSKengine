export module OSKengine.Util.Version;

export namespace OSK {

	/// <summary>
	/// Representa una versión.
	/// </summary>
	struct Version {

		/// <summary>
		/// Versión mayor.
		/// </summary>
		unsigned int mayor;

		/// <summary>
		/// Versión menor.
		/// </summary>
		unsigned int menor;

		/// <summary>
		/// Parche.
		/// </summary>
		unsigned int parche;

	};

}
