#pragma once

#include "DynamicArray.hpp"
#include "Pair.hpp"
#include "BitSet.hpp"

namespace OSK {

	template <typename T> size_t Hash(const T& elem) {
		static std::hash<T> hasher;

		return hasher(elem);
	}

	template <typename TKey, typename TValue, typename TCollection = DynamicArray<Pair<TKey, TValue>>> class HashMap {

		friend class Iterator;

	public:

		typedef Pair<TKey, TValue> TPair;
		typedef TCollection TBucket;

		class Iterator {

			friend class HashMap;

		public:

			Iterator(const HashMap* map, const TBucket* bucket, size_t inBucketIndex, size_t bucketIndex)
				: map(map), bucket(bucket), inBucketIndex(inBucketIndex), bucketIndex(bucketIndex) {

			}

			Iterator operator++() {
				inBucketIndex++;

				while (inBucketIndex >= bucket->GetSize()) {
					inBucketIndex = 0;

					bucketIndex++;

					bucketIndex = map->occupiedBuckets.GetNextTrueIndex(bucketIndex);
					if (bucketIndex == 0) {
						*this = map->end();

						return *this;
					}
					else
						bucket = map->GetBucket(bucketIndex);
				}

				return *this;
			}

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
				return bucketIndex == other.bucketIndex && inBucketIndex == other.inBucketIndex;
			}

			bool operator!= (const Iterator& other) const {
				return bucketIndex != other.bucketIndex || inBucketIndex != other.inBucketIndex;
			}

			TPair& operator*() const {
				return bucket->At(inBucketIndex);
			}

			TPair& GetValue() const {
				return bucket->At(inBucketIndex);
			}

			bool IsEmpty() const {
				return bucket == nullptr;
			}

		private:

			const HashMap* map = nullptr;
			const TBucket* bucket = nullptr;
			size_t inBucketIndex = 0;
			size_t bucketIndex = 0;

		};

		HashMap() {
			occupiedBuckets.SetLength(512);
			for (TSize i = 0; i < 512; i++)
				buckets[i] = TBucket();
		}

		void Insert(const TKey& key, const TValue& value) {
			auto pair = FindPair(key);
			if (pair)
				pair->second = value;
			else {
				auto hash = ConvertHash(Hash(key));
				buckets[hash].Insert(TPair(key, value));
				occupiedBuckets.SetTrue(hash);

				size++;
			}
		}

		void Remove(const TKey& key) {
			KeySearchResult result = _FindKey(key);

			if (result.found) {
				const_cast<TBucket*>(result.it.bucket)->RemoveAt(result.it.inBucketIndex);

				if (result.it.bucket->GetSize() == 0)
					occupiedBuckets.SetFalse(result.it.inBucketIndex);
			}
		}

		const TBucket* GetBucket(size_t index) const {
			return &buckets[index];
		}
		/*DynamicArray<TBucket>& GetAllBuckets() const {
			return buckets;
		}*/

		TValue& Get(const TKey& key) const {
			auto pair = FindPair(key);

			return pair->second;
		}

		bool ContainsKey(const TKey& key) const {
			return _FindKey(key).result;
		}

		Iterator Find(const TKey& key) const {
			auto result = _FindKey(key);

			if (result.found)
				return result.it;

			return end();
		}

		size_t GetSize() const {
			return size;
		}

		size_t GetNumberOfBuckets() const {
			return 512;
		}

		Iterator begin() const {
			if (occupiedBuckets.IsFullFalse())
				return end();

			size_t i = occupiedBuckets.GetNextTrueIndex();
			
			return GetIterator(i, 0);
		}
		Iterator end() const {
			return Iterator(this, nullptr, 0, 512);
		}

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
			auto hash = ConvertHash(Hash(key));
			const TBucket* bucket = &buckets[hash];

			for (TSize i = 0; i < bucket->GetSize(); i++)
				if (bucket->At(i).first == key)
					return { true, GetIterator(hash, i) };

			return { false, end() };
		}

		TPair* FindPair(const TKey& key) const {
			auto result = _FindKey(key);
			if (result.found)
				return &(*result.it);

			return nullptr;
		}

		TSize ConvertHash(TSize hash) const {
			return hash % 512;
		}

		TBucket buckets[512];
		BitSet occupiedBuckets;

		TSize size = 0;

	};

}
