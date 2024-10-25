#pragma once
#include <string>
#include <variant>
#include <optional>
#include <iostream>
#include <vector>
#include "HelperFunctions.hpp"

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

		public:
			JSONObject();
			JSONObject(const std::string& json);
			JSONObject(const std::vector<JSONProperty>& vals);
			JSONObject(const JSONObject& other) = default;
			JSONObject(JSONObject&& other) noexcept = default;
			JSONObject& operator=(const JSONObject& other) = default;
			JSONObject& operator=(JSONObject&& other) noexcept = default;
			~JSONObject() = default;

			template<typename T>
			const std::optional<T> HasProperty(const std::string& propertyName)
			{
				for (const auto& property : PROPERTIES)
				{
					if (property.first == propertyName &&
						typeid(property.second) == typeid(T))
					{
						return property.second;
					}
				}
				return std::nullopt;
			}

			const std::optional<int> TryGetInt(const std::string& propertyName);
			const std::optional<double> TryGetDouble(const std::string& propertyName);
			const std::optional<std::string> TryGetString(const std::string& propertyName);
			const std::optional<JSONObject> TryGetObject(const std::string& propertyName);

			std::string ToString() const;
		};

		class JSONProperty
		{
		public:
			const std::string PROPERTY_NAME;
			const JSONType PROPERTY_VALUE;

			JSONProperty(const std::string& name, const JSONType& type);
			JSONProperty(const JSONProperty& other) = default;
			JSONProperty(JSONProperty&& other) noexcept = default;
			JSONProperty& operator=(const JSONProperty& other) = default;
			JSONProperty& operator=(JSONProperty&& other) noexcept = default;
			~JSONProperty() = default;

		private:
			template<typename T>
			const std::optional<T> TryGetType()
			{
				T* value = std::get_if<T>(&PROPERTY_VALUE);
				if (value != nullptr) return *value;
				else return std::nullopt;
			}

		public:
			const std::optional<int> TryGetInt();
			const std::optional<double> TryGetDouble();
			const std::optional<std::string> TryGetString();
			const std::optional<JSONObject> TryGetObject();
		};

		using JSONSingleType = std::variant<int, double, std::string, JSONObject>;
		using JSONType = std::variant<JSONSingleType, std::vector<JSONSingleType>>;

		std::optional<JSONObject> TryGetJSONFromFile(const std::filesystem::path& path);
	}
}