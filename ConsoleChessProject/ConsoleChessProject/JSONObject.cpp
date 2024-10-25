#include <string>
#include <format>
#include <algorithm>
#include <vector>
#include "JSONUtils.hpp"
#include "HelperFunctions.hpp"
#include "StringUtil.hpp"

namespace Utils
{
	namespace JSON
	{
		JSONObject::JSONObject() : PROPERTIES({}) {}

		JSONObject::JSONObject(const std::vector<JSONProperty>& vals)
			: PROPERTIES(vals) {}

		JSONObject::JSONObject(const std::string& json) :
			PROPERTIES(ConvertJsonToProperties(json)) {}

		std::string JSONObject::CleanString(const std::string& str)
		{
			std::string newStr = Utils::StringUtil(str).trim().ToString();
			newStr = newStr.replace(newStr.begin(), newStr.end(), " ", "");
			return newStr;
		}

		std::optional<JSONSingleType> JSONObject::ParseSingleValue(const std::string& json)
		{
			std::string cleanedJson = CleanString(json);
			const std::string firstStr = cleanedJson.substr(0, 1);
			const std::string lastStr = cleanedJson.substr(cleanedJson.size() - 1, 1);

			if (auto maybeDouble = Utils::TryParse<double>(cleanedJson); maybeDouble.has_value())
				return maybeDouble.value();

			else if (auto maybeInt = Utils::TryParse<int>(cleanedJson); maybeInt.has_value())
				return maybeInt.value();

			else if (firstStr == "\"" && lastStr == "\"")
				return cleanedJson;

			else if (firstStr == "{" && lastStr == "}")
				return ConvertJsonToObject(cleanedJson);
			else
			{
				std::string error = std::format("Tried to parse JSON {} for a single value, but"
					"it could not be converted to any recognizable type", json);
				Utils::Log(Utils::LogType::Error, error);
				return std::nullopt;
			}
		}

		std::vector<JSONSingleType> JSONObject::ParseArray(const std::string& jsonArr)
		{
			std::string currentJson = CleanString(jsonArr);

			if (currentJson.substr(0, 1) != "[" ||
				currentJson.substr(currentJson.size() - 1, 1) != "]")
				return {};

			currentJson = currentJson.substr(1, currentJson.size() - 2);
			int leftIndex = 1;
			int rightIndex = currentJson.find(",");
			std::string currentStr = currentJson.substr(leftIndex, rightIndex - leftIndex);

			std::vector<JSONSingleType> resultArr;
			while (leftIndex >= 0 && leftIndex < currentJson.size() &&
				rightIndex >= 0 && rightIndex < currentJson.size() && leftIndex < rightIndex)
			{
				currentStr = currentJson.substr(leftIndex, rightIndex - leftIndex);
				std::optional<JSONSingleType> element = ParseSingleValue(currentStr);
				if (element.has_value()) resultArr.push_back(element.value());

				int leftIndex = rightIndex + 1;
				int rightIndex = currentJson.find(",", leftIndex);
			}
			return resultArr;
		}

		std::vector<JSONProperty> JSONObject::ConvertJsonToProperties(const std::string& json)
		{
			std::string currentJson = CleanString(json);
			std::string firstStr = currentJson.substr(0, 1);
			std::string lastStr = currentJson.substr(currentJson.size() - 1, 1);

			if (firstStr != "{" || lastStr != "}")
			{
				std::string error = std::format("Tried to parse JSON {} but could not find object start", json);
				Utils::Log(Utils::LogType::Warning, error);
				return {};
			}

			int leftIndex = currentJson.find("\"");
			int rightIndex = currentJson.find("\"", leftIndex + 1);

			//If the start of the first property is not at the character after { or does not exist
			//or the right end of the first property name does not exist, we know it is invalid
			if (leftIndex <= 1 || rightIndex < 0 || rightIndex <= leftIndex)
			{
				std::string error = std::format("Tried to parse JSON {} but could not first property", json);
				Utils::Log(Utils::LogType::Warning, error);
				return {};
			}

			int scopeLevel = 0;
			std::string propertyName;
			std::vector<JSONProperty> properties;
			//Value str should not include the right index since it is "," not value segment
			std::string valueStr;

			for (int i = rightIndex; i < currentJson.size() - 1; i++)
			{
				if (rightIndex <= leftIndex)
				{
					std::string error = std::format("Tried to parse JSON {} but found greater "
						"left when parsing value. Left: {} Right: {}", json, leftIndex, rightIndex);
					Utils::Log(Utils::LogType::Error, error);
					return {};
				}

				firstStr = currentJson.substr(i, 1);
				if (firstStr == "{" || firstStr == "[") scopeLevel++;
				else if (firstStr == "}" || firstStr == "]") scopeLevel--;
				//If we find a " it means we have a property name (or property value)
				//so we check if we have no property right now and set it to it
				else if (firstStr == "\"" && propertyName.size() == 0)
				{
					int propertyEndIndex = currentJson.find("\"", i + 1);
					if (propertyEndIndex >= 0)
					{
						propertyName = currentJson.substr(i + 1, propertyEndIndex - i);
						i += (propertyEndIndex - i);
					}
				}
				//If we find : at the desired level it means value for property name starts, so we 
				//set up pointer to search
				else if (scopeLevel == 0 && firstStr == ":")
				{
					leftIndex = i + 1;
				}


				if (scopeLevel < 0)
				{
					std::string error = std::format("Tried to parse JSON {} but managed "
						"to get to scope level <0", json);
					Utils::Log(Utils::LogType::Error, error);
					return {};
				}
				//If the scope level is not the right one, we keep going 
				//and adding to the right index in case we have nesting
				if (scopeLevel != 0)
				{
					continue;
				}
				else if (scopeLevel == 0 && (propertyName.size() == 0 ||
					firstStr != "," || i != currentJson.size() - 2))
				{
					continue;
				}

				//If we get to this point it means we have property name, right scope,
				//so we just use the current index to get right index for value
				if (i == currentJson.size() - 2) rightIndex = i;
				else rightIndex = i - 1;

				std::string valueStr = currentJson.substr(leftIndex, rightIndex - leftIndex + 1);
				if (auto maybeSingleType = ParseSingleValue(valueStr); maybeSingleType.has_value())
					properties.push_back({ propertyName, maybeSingleType.value() });
				else if (auto arrResult = ParseArray(valueStr); arrResult.size() > 0)
					properties.push_back({ propertyName, arrResult });
				else
				{
					std::string error = std::format("Tried to parse JSON {} but could not parse "
						"the value: {} for property {}", json, propertyName, valueStr);
					Utils::Log(Utils::LogType::Error, error);
					return {};
				}

				propertyName = "";
			}
			return properties;
		}

		JSONObject JSONObject::ConvertJsonToObject(const std::string& json)
		{
			auto properties = ConvertJsonToProperties(json);
			return { properties };
		}

		const std::optional<int> JSONObject::TryGetInt(const std::string& propertyName)
		{
			return HasProperty<int>(propertyName);
		}

		const std::optional<double> JSONObject::TryGetDouble(const std::string& propertyName)
		{
			return HasProperty<double>(propertyName);
		}

		const std::optional<std::string> JSONObject::TryGetString(const std::string& propertyName)
		{
			return HasProperty<std::string>(propertyName);
		}

		const std::optional<JSONObject> JSONObject::TryGetObject(const std::string& propertyName)
		{
			return HasProperty<JSONObject&>(propertyName);
		}

		std::string JSONObject::ToString() const
		{
			if (PROPERTIES.size()) return "[]";
			std::string result = "[";
			std::string propertyStr;

			int index = 0;
			for (const auto& property : PROPERTIES)
			{
				propertyStr = std::format("({},{})", property.PROPERTY_NAME, property.PROPERTY_VALUE);
				result += propertyStr;

				if (index >= PROPERTIES.size() - 1) continue;

				result += ", ";
				index++;
			}

			result += "]";
			return result;
		}
	}
}