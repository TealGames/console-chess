#include <unordered_map>
#include <map>
#include <vector>
#include <iterator>
#include "HelperFunctions.hpp"

namespace Utils
{
	namespace Collections
	{
		template<typename T>
		class FrozenVector
		{
		private:
			using VectorType = std::vector<T>;
			const VectorType& VECTOR;

		private:
			bool IsOutOfBounds(int index)
			{
				return index < 0 || index >= VECTOR.Size();
			}

		/*public:
			class FrozenVectorIterator
			{
			private:
				int m_CurrentIndex = -1;
				const FrozenVector<T>& VECTOR;

				int IndexInBoundsOrDefault(int index)
				{
					if (IsOutOfBounds(index)) return 0;
					return index;
				}

			public:
				FrozenVectorIterator(const FrozenVector<T>& vector, const int index)
					: VECTOR(vector), m_CurrentIndex(IndexInBoundsOrDefault(index)) {}

				const T& operator*() const
				{
					int index = IndexInBoundsOrDefault(m_CurrentIndex);
					return VECTOR[index];
				}

				FrozenVectorIterator& operator++() const
				{
					m_CurrentIndex++;
					if (m_CurrentIndex >= VECTOR.Size()) m_CurrentIndex = VECTOR.Size() - 1;
					else if (m_CurrentIndex < 0) index = 0;
					return *this;
				}

				bool operator==(const FrozenVectorIterator& it) const
				{
					return m_CurrentIndex == it.m_CurrentIndex &&
						(&VECTOR == &it.VECTOR || VECTOR == it.VECTOR);
				}

				bool operator!=(const FrozenVectorIterator& it) const
				{
					return !(*this == it);
				}
			};*/

		public:
			FrozenVector(const std::vector<T>& vec) : VECTOR(vec) {}

			const T& operator[](const int index) const
			{
				if (IsOutOfBounds(index))
				{
					std::string err = std::format("Index {} out bounds: [0, {}] when trying to retrieve "
						"element from frozen vector", index, VECTOR.size());
					throw std::out_of_range(error);
				}

				return VECTOR[index];
			}

			inline const T& at(const int index) const
			{
				return *this[index];
			}

			inline size_t Size() const
			{
				return VECTOR.size();
			}

			inline bool Contains(T element) const
			{
				return VECTOR.find(element) != VECTOR.end();
			}

			inline VectorType::const_iterator begin() const
			{
				return VECTOR.begin();
			}

			inline VectorType::const_iterator end() const
			{
				return VECTOR.end();
			}
		};
	}
}