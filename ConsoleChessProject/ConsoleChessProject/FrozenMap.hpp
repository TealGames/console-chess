#include <unordered_map>
#include <map>
#include <vector>
#include <iterator>
#include "HelperFunctions.hpp"

namespace Utils
{
	namespace Collections
	{
		template<typename KType, typename VType>
		class FrozenMap
		{
		private:
			using MapType = std::unordered_map<KType, VType>;
			const MapType& MAP;
			
		public:
			FrozenMap(const std::unordered_map<KType, VType>& map) : MAP(map) {}
			FrozenMap(const std::map<KType, VType>& map) : MAP(map) {}
			FrozenMap(const std::vector<KType>& keys, const std::vector<VType>& vals)
				: MAP(Utils::GetMapFromVectors<KType, VType>(keys, vals)) {}

			const VType& operator[](const KType& key) const
			{
				auto it = MAP.find(key);
				if (it == MAP.end()) 
				{
					throw std::out_of_range("Key for frozen map not found");
				}
				return it->second;
			}

			inline const T& at(const KType& key) const
			{
				return *this[key];
			}

			inline size_t Size() const
			{
				return MAP.size();
			}

			inline bool Contains(KType key) const
			{
				return MAP.find(key) != MAP.end();
			}

			inline MapType::const_iterator begin() const
			{
				return MAP.begin();
			}

			inline MapType::const_iterator end() const
			{
				return MAP.begin();
			}
		};
	}
}