#pragma once

#include "DynamicArray.hpp"
#include "Pair.hpp"
#include "ConstexprBitSet.hpp"

namespace OSK {

	template <typename T> size_t Hash(const T& elem) {
		static std::hash<T> hasher;

		return hasher(elem);
	}


	/// <summary>
	/// Un HashMap representa una colleci�n que enlaza un valor a otro.
	/// Est� compuesto internamente por varias colecciones que almacenan los datos.
	/// 
	/// @warning No proporciona estabilidad de puntero.
	/// </summary>
	/// <typeparam name="TKey">Tipo de valor al que se le enlaza otro.</typeparam>
	/// <typeparam name="TValue">Tipo de valor enlazado.</typeparam>
	/// <typeparam name="numBuckets">N�mero de cubos.</typeparam>
	/// <typeparam name="TCollection">Tipo de colecci�n de cada cubo.</typeparam>
	template <typename TKey, typename TValue, TSize numBuckets = 512> class HashMap {

		friend class Iterator;

	public:

		/// <summary>
		/// Pareja valor llave -> valor enlazado.
		/// </summary>
		typedef Pair<TKey, TValue> TPair;

		/// <summary>
		/// Tipo de colecci�n de cada cubo.
		/// </summary>
		typedef DynamicArray<Pair<TKey, TValue>> TBucket;


		/// <summary>
		/// Un iterador apunta a una pareja de la colecci�n.
		/// Se puede recorrer la colecci�n a trav�s de los iteradores.
		/// 
		/// @warning Para que el iterador se pueda usar, la colecci�n debe estar guardada en un lugar
		/// que permita estabilidad de punteros.
		/// @warning Adem�s, la colecci�n no se debe modificar (ni a�adiendo ni eliminando elementos) mientras
		/// se use un iterador, para asegurarnos de que siga siendo v�lido.
		/// </summary>
		class Iterator {

			friend class HashMap;

		public:

			/// <summary>
			/// Crea el iterador apuntando a un elemento (o a ninguno).
			/// </summary>
			/// <param name="map">HashMap del iterador.</param>
			/// <param name="bucket">Cubo al que pertenece la pareja.</param>
			/// <param name="inBucketIndex">�ndice de la pareja dentro del cubo.</param>
			/// <param name="bucketIndex">�ndice del cubo dentro del array de cubos.</param>
			Iterator(const HashMap* map, const TBucket* bucket, TSize inBucketIndex, TSize bucketIndex)
				: map(map), bucket(bucket), inBucketIndex(inBucketIndex), bucketIndex(bucketIndex) {

			}

			/// <summary>
			/// Devuelve un iterador que apunta a la siguiente pareja de la colecci�n.
			/// 
			/// @note Si hemos llegado al final, no contin�a.
			/// </summary>
			Iterator operator++() {
				inBucketIndex++;

				// Si ya no quedan m�s elementos en el cubo actual,
				// pasamos al siguiente cubo.
				while (inBucketIndex >= bucket->GetSize()) {
					inBucketIndex = 0;

					bucketIndex++;

					bucketIndex = map->occupiedBuckets.GetNextTrueIndex(bucketIndex);
					if (bucketIndex == 0) {
						// Si hemos vuelto al principio, hemos terminado de recorrer la colecci�n.
						*this = map->end();

						return *this;
					}
					else {
						bucket = map->GetBucket(bucketIndex);
					}
				}

				return *this;
			}

			/// <summary>
			/// Devuelve un iterador que apunta a la anterior pareja de la colecci�n.
			/// 
			/// @note Si hemos llegado al principio, no contin�a.
			/// </summary>
			Iterator operator--() {
				if (inBucketIndex > 0) {
					inBucketIndex--;

					return *this;
				}

				while (bucket->GetSize() == 0) {
					bucketIndex--;

					if (bucketIndex > 0)
						bucket = map->GetBucket(bucketIndex);
					else
						return map->begin();

					bucketIndex = bucket->GetSize() - 1;
				}

				return *this;
			}

			bool operator> (const Iterator& other) const {
				if (bucketIndex == other.bucketIndex)
					return inBucketIndex > other.inBucketIndex;

				return bucketIndex > other.bucketIndex;
			}

			bool operator< (const Iterator& other) const {
				if (bucketIndex == other.bucketIndex)
					return inBucketIndex < other.inBucketIndex;

				return bucketIndex < other.bucketIndex;
			}

			bool operator>= (const Iterator& other) const {
				if (bucketIndex == other.bucketIndex)
					return inBucketIndex >= other.inBucketIndex;

				return bucketIndex >= other.bucketIndex;
			}

			bool operator<= (const Iterator& other) const {
				if (bucketIndex == other.bucketIndex)
					return inBucketIndex <= other.inBucketIndex;

				return bucketIndex <= other.bucketIndex;
			}

			bool operator== (const Iterator& other) const {
				return bucketIndex == other.bucketIndex && inBucketIndex == other.inBucketIndex && bucket == other.bucket;
			}

			bool operator!= (const Iterator& other) const {
				return bucketIndex != other.bucketIndex || (inBucketIndex != other.inBucketIndex && bucket != other.bucket);
			}

			/// <summary>
			/// Devuelve la pareja apuntada por el iterador.
			/// </summary>
			TPair& operator*() const {
				return bucket->At(inBucketIndex);
			}

			/// <summary>
			/// Devuelve la pareja apuntada por el iterador.
			/// </summary>
			TPair& GetValue() const {
				return bucket->At(inBucketIndex);
			}

			/// <summary>
			/// Comprueba si este iterador NO apunta a un elemento v�lido.
			/// </summary>
			bool IsEmpty() const {
				return bucket == nullptr;
			}

		private:

			const HashMap* map = nullptr;
			const TBucket* bucket = nullptr;
			TSize inBucketIndex = 0;
			TSize bucketIndex = 0;

		};


		/// <summary>
		/// Construye un hashmap vac�o.
		/// </summary>
		HashMap() {

		}

		/// <summary>
		/// Inserta una nueva pareja.
		/// 
		/// @note Si la pareja ya exist�a, se actualiza el segundo valor.
		/// </summary>
		/// <param name="key">Valor llave.</param>
		/// <param name="value">Valor enlazado.</param>
		void InsertCopy(const TKey& key, const TValue& value) {
			auto pair = FindPair(key);
			if (pair) {
				pair->second = value;
			}
			else {
				const TSize hash = ConvertHash(static_cast<TSize>(Hash(key)));
				buckets[hash].Insert(TPair(key, value));
				occupiedBuckets.SetTrue(hash);

				size++;
			}
		}

		/// <summary>
		/// Inserta una nueva pareja.
		/// 
		/// @note Si la pareja ya exist�a, se actualiza el segundo valor.
		/// </summary>
		/// <param name="key">Valor llave.</param>
		/// <param name="value">Valor enlazado.</param>
		void Insert(const TKey& key, const TValue& value) {
			InsertCopy(key, value);
		}

		/// <summary>
		/// Inserta una nueva pareja.
		/// 
		/// @note Si la pareja ya exist�a, se actualiza el segundo valor.
		/// </summary>
		/// <param name="key">Valor llave.</param>
		/// <param name="value">Valor enlazado.</param>
		void InsertMove(const TKey& key, TValue&& value) {
			TPair* pair = FindPair(key);

			if (pair) {
				//pair->second = std::move(value);
				Remove(key);
				InsertMove(key, std::move(value));
			}
			else {
				auto hash = ConvertHash(static_cast<TSize>(Hash(key)));
				buckets[hash].InsertMove(std::move(TPair(key, std::move(value))));
				occupiedBuckets.SetTrue(hash);

				size++;
			}
		}

		/// <summary>
		/// Inserta una nueva pareja.
		/// 
		/// @note Si la pareja ya exist�a, se actualiza el segundo valor.
		/// </summary>
		/// <param name="key">Valor llave.</param>
		/// <param name="value">Valor enlazado.</param>
		void Insert(const TKey& key, TValue&& value) {
			InsertMove(key, std::move(value));
		}

		/// <summary>
		/// Elimina una pareja.
		/// </summary>
		/// <param name="key">Valor llave de la pareja.</param>
		/// 
		/// @note Si no existe la pareja, no ocurre nada.
		void Remove(const TKey& key) {
			KeySearchResult result = _FindKey(key);

			if (result.found) {
				const_cast<TBucket*>(result.it.bucket)->RemoveAt(result.it.inBucketIndex);

				if (result.it.bucket->GetSize() == 0)
					occupiedBuckets.SetFalse(result.it.bucketIndex);
			}
		}

		/// <summary>
		/// Devuelve un puntero al cubo en el �ndice dado.
		/// </summary>
		const TBucket* GetBucket(TSize index) const {
			return &buckets[index];
		}

		/// <summary>
		/// Devuelve el valor enlazado del valor llave dado.
		/// 
		/// @pre Debe existir, de lo contrario habr� un error.
		/// </summary>
		TValue& Get(const TKey& key) const {
			auto pair = FindPair(key);
			OSK_ASSERT(pair != nullptr, "No se encontr� el valor.");

			return pair->second;
		}

		/// <summary>
		/// Devuelve una copia del valor enlazado al valor llave dado.
		/// 
		/// @pre Debe existir, de lo contrario habr� un error.
		/// </summary>
		TValue GetCopy(const TKey& key) const {
			auto pair = FindPair(key);
			OSK_ASSERT(pair, "No se encontr� el valor.");

			return pair->second;
		}

		/// <summary>
		/// Comprueba si existe una apreja con el valor llave dado.
		/// </summary>
		bool ContainsKey(const TKey& key) const {
			return _FindKey(key).found;
		}

		/// <summary>
		/// Devuelve el iterador apuntando al elemento buscado.
		/// 
		/// @note Si no existe dicho elemento, se devuelve un iterador vac�o (pero v�lido).
		/// </summary>
		Iterator Find(const TKey& key) const {
			auto result = _FindKey(key);

			if (result.found)
				return result.it;

			return end();
		}

		/// <summary>
		/// Devuelve el n�mero de parejas almacenadas.
		/// </summary>
		TSize GetSize() const {
			return size;
		}

		/// <summary>
		/// Devuelve el n�mero de cubos.
		/// </summary>
		TSize GetNumberOfBuckets() const {
			return numBuckets;
		}


		/// <summary>
		/// Devuelve un iterador que apunta a la primera pareja.
		/// 
		/// @note Si no hay ninguna pareja, devuelve 'end()'.
		/// </summary>
		Iterator begin() const {
			if (occupiedBuckets.IsAllFalse())
				return end();

			const TSize i = occupiedBuckets.GetNextTrueIndex();
			
			return GetIterator(i, 0);
		}

		/// <summary>
		/// Devuelve un iterador vac�o.
		/// </summary>
		Iterator end() const {
			return Iterator(this, nullptr, 0, numBuckets);
		}

		/// <summary>
		/// Devuelve un iterador apuntando a la pareja en el cubo y en la posici�n dadas.
		/// 
		/// @warning Puede dar error si se introducen �ndices incorrectos.
		/// </summary>
		Iterator GetIterator(TSize bucketId, TSize inbucketId) const {
			auto it = Iterator(this, nullptr, inbucketId, bucketId);
			it.bucket = (TBucket*)&buckets[bucketId];

			return it;
		}

	private:

		struct KeySearchResult {
			bool found = false;
			Iterator it;
		};

		KeySearchResult _FindKey(const TKey& key) const {
			auto hash = ConvertHash(static_cast<TSize>(Hash(key)));
			const TBucket* bucket = &buckets[hash];

			for (TSize i = 0; i < bucket->GetSize(); i++)
				if (bucket->At(i).first == key)
					return { true, GetIterator(hash, i) };

			return { false, end() };
		}

		/// <summary>
		/// Busca una pareja con la llave dada.
		/// Si no existe, devuelve null.
		/// </summary>
		TPair* FindPair(const TKey& key) const {
			auto result = _FindKey(key);
			if (result.found)
				return &(*result.it);

			return nullptr;
		}

		/// <summary>
		/// Convierte un hash para obtener el �ndice
		/// del cubo.
		/// </summary>
		TSize ConvertHash(TSize hash) const {
			return hash % numBuckets;
		}

		TBucket buckets[numBuckets];
		ConstexprBitSet<numBuckets> occupiedBuckets;

		TSize size = 0;

	};

}
