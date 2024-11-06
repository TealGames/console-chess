#pragma once
#include <vector>
#include "JSONUtils.hpp"

namespace Utils
{
	namespace JSON
	{
		class JSONObject
		{
		private:
			const std::vector<JSONProperty> PROPERTIES;

			std::string CleanString(const std::string& str);

			std::optional<JSONSingleType> ParseSingleValue(const std::string& json);
			std::vector<JSONSingleType> ParseArray(const std::string& json);

			std::vector<JSONProperty> ConvertJsonToProperties(const std::string& json);
			JSONObject ConvertJsonToObject(const std::string& json);

			template<typename T>
			const std::optional<T> HasProperty(const std::string& propertyName) const
			{
				for (const auto& property : PROPERTIES)
				{
					if (property.PROPERTY_NAME == propertyName)
					{
						return property.TryGetType<T>();
					}
				}
				return std::nullopt;
			}

		public:
			JSONObject();
			JSONObject(const std::string& json);
			JSONObject(const std::vector<JSONProperty>& vals);
			JSONObject(const JSONObject& other) = default;
			JSONObject(JSONObject&& other) noexcept = default;
			JSONObject& operator=(const JSONObject& other) = default;
			JSONObject& operator=(JSONObject&& other) noexcept = default;
			~JSONObject() = default;

			const std::optional<int> TryGetInt(const std::string& propertyName) const;
			const std::optional<double> TryGetDouble(const std::string& propertyName) const;
			const std::optional<std::string> TryGetString(const std::string& propertyName) const;
			const std::optional<JSONObject> TryGetObject(const std::string& propertyName) const;
			const std::optional<JSONList> TryGetList(const std::string& propertyName) const;

			std::string ToString() const;
		};
	}
}
