#pragma once

#include "OSKmacros.h"
#include "DynamicArray.hpp"
#include "HashMap.hpp"
#include "LinkedList.hpp"

#include <string>

#ifndef OSK_ASSET_TYPE_REG
#define OSK_ASSET_TYPE_REG(typeName) const static std::string GetAssetType() { return typeName; }
#endif

namespace OSK {

	/// <summary>
	/// Un asset es un archivo/contenido que se usa en un juego. Incluye
	/// contenido como texturas, modelos 3D, efectos de sonido, música, etc...
	/// 
	/// Para poder manejar su ciclo de vida, los assets se agrupan en grupos con
	/// lifetimes idénticos. Una vez que se termine el ciclo de vida, todos los
	/// assets con ese lifetime se eliminan.
	/// 
	/// Se pueden crear assets específicos para cada juego. La clase asset debe tener
	/// definido el tipo de asset (un string identificador de tipo único) usando el
	/// macro OSK_ASSET_TYPE_REG.
	/// </summary>
	class OSKAPI_CALL IAsset {

	public:

		virtual ~IAsset() = default;

		/// <summary>
		/// Establece el nombre de la instancia de este asset.
		/// Es un identificador único.
		/// </summary>
		void SetName(const std::string& name);

		/// <summary>
		/// Nombre de la instancia del asset.
		/// </summary>
		std::string GetName() const;

		/// <summary>
		/// Obtiene la ruta del archivo .json que
		/// describe esta instancia del asset.
		/// </summary>
		std::string GetAssetFilename() const;

	protected:

		IAsset(const std::string& assetFile);

	private:

		std::string name;
		std::string assetFileName;

	};
	
}
