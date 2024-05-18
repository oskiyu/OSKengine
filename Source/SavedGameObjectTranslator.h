#pragma once

#include "OSKmacros.h"
#include "HashMap.hpp"

#include "GameObject.h"

namespace OSK::ECS {

	/// @brief Tabla que permite obtener el índice actual de una entidad
	/// que ha sido guardada en disco.
	/// 
	/// Al spawnear la entidad, es posible que su nuevo ID no coincida con
	/// el ID previamente guardado.
	class OSKAPI_CALL SavedGameObjectTranslator {

	public:

		/// @brief Añade una entrada a la tabla.
		/// @param savedIndex Índice guardado.
		/// @param currentIndex Nuevo índice.
		/// 
		/// @pre @p savedIndex no debe estar vacío.
		/// @pre @p currentIndex no debe estar vacío.
		/// 
		/// @note Si el índicce @p savedIndex ya estaba presente,
		/// su valor se sobreescribe.
		void AddObject(
			GameObjectIndex savedIndex,
			GameObjectIndex currentIndex);

		/// @param savedIndex Índice guardado.
		/// @return Índice actual para el objeto guardado.
		/// 
		/// @note Si @p savedIndex está vacío, devuelve un identificador vacío.
		GameObjectIndex GetCurrentIndex(GameObjectIndex savedIndex) const;

		/// @param savedIndex Índice guardado.
		/// @return True si tiene una entrada con el índice @p savedIndex.
		bool HasSavedIndex(GameObjectIndex savedIndex) const;

	private:

		std::unordered_map<GameObjectIndex, GameObjectIndex> m_savedToCurrent;

	};

}
