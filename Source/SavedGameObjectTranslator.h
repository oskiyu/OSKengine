#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"

#include "GameObject.h"

namespace OSK::ECS {

	/// @brief Tabla que permite obtener el �ndice actual de una entidad
	/// que ha sido guardada en disco.
	/// 
	/// Al spawnear la entidad, es posible que su nuevo ID no coincida con
	/// el ID previamente guardado.
	class OSKAPI_CALL SavedGameObjectTranslator {

	public:

		/// @brief A�ade una entrada a la tabla.
		/// @param savedIndex �ndice guardado.
		/// @param currentIndex Nuevo �ndice.
		/// 
		/// @pre @p savedIndex no debe estar vac�o.
		/// @pre @p currentIndex no debe estar vac�o.
		/// 
		/// @note Si el �ndicce @p savedIndex ya estaba presente,
		/// su valor se sobreescribe.
		void AddObject(
			GameObjectIndex savedIndex,
			GameObjectIndex currentIndex);

		/// @param savedIndex �ndice guardado.
		/// @return �ndice actual para el objeto guardado.
		/// 
		/// @note Si @p savedIndex est� vac�o, devuelve un identificador vac�o.
		GameObjectIndex GetCurrentIndex(GameObjectIndex savedIndex) const;

		/// @param savedIndex �ndice guardado.
		/// @return True si tiene una entrada con el �ndice @p savedIndex.
		bool HasSavedIndex(GameObjectIndex savedIndex) const;

	private:

		std::unordered_map<GameObjectIndex, GameObjectIndex> m_savedToCurrent;

	};

}
