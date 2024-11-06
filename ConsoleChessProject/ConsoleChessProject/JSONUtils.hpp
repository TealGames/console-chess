#pragma once
#include <string>
#include <format>
#include <variant>
#include <optional>
#include <iostream>
#include <vector>
#include "HelperFunctions.hpp"

namespace Utils
{
	namespace JSON
	{
		static const std::string JSON_EXTENSION = "json";

		class JSONProperty;
		class JSONObject
		{
		private:
			const std::vector<JSONProperty> PROPERTIES;

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
			const std::optional<T> HasProperty(const std::string& propertyName) const;

			/*const std::optional<int> TryGetInt(const std::string& propertyName) const;
			const std::optional<double> TryGetDouble(const std::string& propertyName) const;
			const std::optional<std::string> TryGetString(const std::string& propertyName) const;
			const std::optional<JSONObject> TryGetObject(const std::string& propertyName) const;
			const std::optional<JSONList> TryGetList(const std::string& propertyName) const;*/

			std::string ToString() const;
		};

		using JSONSingleType = std::variant<int, double, std::string, JSONObject>;
		using JSONList = std::vector<JSONSingleType>;

		template<typename T>
		inline bool IsListType()
		{
			return Utils::AreSameType<T, std::vector<JSONSingleType>>();
		}

		inline std::optional<int> TryGetInt(const JSONObject& obj, const std::string& propertyName);
		inline std::optional<double> TryGetDouble(const JSONObject& obj, const std::string& propertyName);
		inline std::optional<std::string> TryGetString(const JSONObject& obj, const std::string& propertyName);
		inline std::optional<JSONObject> TryGetObject(const JSONObject& obj, const std::string& propertyName);
		inline std::optional<JSONList> TryGetList(const JSONObject& obj, const std::string& propertyName);

		std::string CleanJSON(const std::string& json);
		std::string JSONSingleTypeToString(const JSONSingleType& type);
		std::string JSONListToString(const JSONList& list);
		std::optional<JSONObject> TryGetJSONFromFile(const std::filesystem::path& path);

		class JSONProperty
		{
		public:
			const std::string PROPERTY_NAME;
			const std::vector<JSONSingleType> PROPERTY_VALUE;
			const bool IS_LIST;

			JSONProperty(const std::string& name, const JSONSingleType& type);
			JSONProperty(const std::string& name, const JSONList& type);
			JSONProperty(const JSONProperty& other) = default;
			JSONProperty(JSONProperty&& other) noexcept = default;
			JSONProperty& operator=(const JSONProperty& other) = default;
			JSONProperty& operator=(JSONProperty&& other) noexcept = default;
			~JSONProperty() = default;
			
		private:
			const std::optional<JSONSingleType> TryGetSingleType() const;

		public:
			template<typename T>
			std::enable_if_t<std::is_same_v<T, JSONList>, std::optional<T>> TryGetType() const
			{
				if (IS_LIST) {
					return PROPERTY_VALUE;
				}
				return std::nullopt;
			}

			template<typename T>
			std::enable_if_t<!std::is_same_v<T, JSONList>, std::optional<T>> TryGetType() const
			{
				//If it is not a list, it should be a single type
				std::optional<JSONSingleType> maybeSingleType = TryGetSingleType();
				if (!maybeSingleType.has_value())
				{
					return std::nullopt;
				}

				JSONSingleType singleType = maybeSingleType.value();
				T* pointer = std::get_if<T>(&singleType);
				if (pointer!=nullptr)
				{
					return *pointer;
				}

				return std::nullopt;
			}

			std::string ToString() const;
		};
	}
}