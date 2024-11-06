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
		//Forward delcarations
		static std::optional<JSONSingleType> ParseSingleValue(const std::string& json);
		static JSONObject ConvertJsonToObject(const std::string& json);

		static std::vector<JSONSingleType> ParseArray(const std::string& jsonArr)
		{
			std::string currentJson = CleanJSON(jsonArr);

			if (currentJson.substr(0, 1) != "[" ||
				currentJson.substr(currentJson.size() - 1, 1) != "]")
				return {};

			currentJson = currentJson.substr(1, currentJson.size() - 2);
			size_t leftIndex = 1;
			size_t rightIndex = currentJson.find(",");
			std::string currentStr = currentJson.substr(leftIndex, rightIndex - leftIndex);

			std::vector<JSONSingleType> resultArr;
			while (leftIndex >= 0 && leftIndex < currentJson.size() &&
				rightIndex >= 0 && rightIndex < currentJson.size() && leftIndex < rightIndex)
			{
				currentStr = currentJson.substr(leftIndex, rightIndex - leftIndex);
				std::optional<JSONSingleType> element = ParseSingleValue(currentStr);
				if (element.has_value()) resultArr.push_back(element.value());

				size_t leftIndex = rightIndex + 1;
				size_t rightIndex = currentJson.find(",", leftIndex);
			}
			return resultArr;
		}

		static std::optional<JSONSingleType> ParseSingleValue(const std::string& json)
		{
			std::string cleanedJson = CleanJSON(json);
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

		static size_t FindStringEnd(std::string str, int startIndex)
		{
			size_t resultIndex = str.find("\"", startIndex);
			if (resultIndex == std::string::npos) return std::string::npos;

			//If it is has a string escape it means it is a string " part of the string
			//and it is not the end
			while (resultIndex!=std::string::npos && resultIndex < str.size() - 1 && resultIndex>0 
				&& str.substr(resultIndex - 1, 1) == "\\")
			{
				resultIndex = str.find("\"", resultIndex+1);
			}
			return resultIndex;
		}

		static std::vector<JSONProperty> ConvertJsonToProperties(const std::string& json)
		{
			std::string currentJson = CleanJSON(json);
			std::string firstStr = currentJson.substr(0, 1);
			std::string lastStr = currentJson.substr(currentJson.size() - 1);

			if (firstStr != "{" || lastStr != "}")
			{
				std::string error = std::format("Tried to parse JSON {} but could not find object start", json);
				Utils::Log(Utils::LogType::Warning, error);
				return {};
			}

			size_t leftIndex = currentJson.find("\"");
			size_t rightIndex = currentJson.find("\"", leftIndex + 1);

			//If the start of the first property is not at the character after { or does not exist
			//or the right end of the first property name does not exist, we know it is invalid
			if (leftIndex < 1 || rightIndex < 0 || rightIndex <= leftIndex)
			{
				std::string error = std::format("Tried to parse JSON {} converted: {} "
					"but could not first property", json, currentJson);
				Utils::Log(Utils::LogType::Warning, error);
				return {};
			}

			int scopeLevel = 0;
			std::string propertyName;
			std::vector<JSONProperty> properties;
			//Value str should not include the right index since it is "," not value segment
			std::string valueStr;

			rightIndex = leftIndex;
			while (rightIndex<currentJson.size())
			{
				std::cout << "LOOP"<<propertyName<<" ";
				if (rightIndex < leftIndex)
				{
					std::string error = std::format("Tried to parse JSON {} but found greater "
						"left when parsing value. Left: {} Right: {}", json, leftIndex, rightIndex);
					Utils::Log(Utils::LogType::Error, error);
					return {};
				}

				firstStr = currentJson.substr(rightIndex, 1);
				if (firstStr == "{" || firstStr == "[") scopeLevel++;
				else if (firstStr == "}" || firstStr == "]") scopeLevel--;
				//If we find a " it means we have a property name (or property value)
				//so we check if we have no property right now and set it to it
				else if (firstStr == "\"" && propertyName.size() == 0)
				{
					size_t propertyEndIndex = FindStringEnd(currentJson, rightIndex + 1);
					if (propertyEndIndex ==std::string::npos)
					{
						std::string error = std::format("Tried to parse JSON: {} "
							"but could not matching string quotes at index: {}", currentJson, rightIndex);
						Utils::Log(Utils::LogType::Warning, error);
						return {};
					}
					else if (propertyEndIndex == currentJson.size() - 1 || 
						currentJson.substr(propertyEndIndex+1, 1)!=":")
					{
						std::string error = std::format("Tried to parse JSON: {} "
							"but encountered string value (meaning property parsing failed) at {}", currentJson, rightIndex);
						Utils::Log(Utils::LogType::Warning, error);
						return {};
					}
					//Since we only care about the local ones we need to worry about
					//those properties only and recursion handles nested properties
					else if (scopeLevel==0)
					{
						propertyName = currentJson.substr(rightIndex + 1, propertyEndIndex - rightIndex - 1);
						std::cout << "found property" << propertyName << rightIndex;
						rightIndex += (propertyEndIndex - rightIndex);
						leftIndex = rightIndex + 1;
					}
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
				else if (scopeLevel != 0)
				{
					rightIndex++;
					continue;
				}
				/*else if (scopeLevel == 0 && (propertyName.size() == 0 ||
					firstStr != "," || rightIndex != currentJson.size() - 2))*/
				else if (scopeLevel == 0 && (propertyName.size() == 0 ||
					firstStr != ","))
				{
					//std::cout << "false scope 0";
					rightIndex++;
					continue;
				}

				//If we get to this point it means we have property name, right scope,
				//so we just use the current index to get right index for value
				if (rightIndex != currentJson.size() - 2) rightIndex--;

				std::string valueStr = currentJson.substr(leftIndex, rightIndex - leftIndex + 1);
				std::cout << "VAL" << valueStr;
				if (auto maybeSingleType = ParseSingleValue(valueStr); maybeSingleType.has_value())
				{
					std::cout << "Adding property";
					properties.push_back({ propertyName, maybeSingleType.value() });
				}
					
				else if (auto arrResult = ParseArray(valueStr); arrResult.size() > 0)
					properties.push_back({ propertyName, arrResult});
				else
				{
					std::string error = std::format("Tried to parse JSON {} but could not parse "
						"the value: {} for property {}", json, propertyName, valueStr);
					Utils::Log(Utils::LogType::Error, error);
					return {};
				}

				if (rightIndex)
				propertyName = "";
			}
			return properties;
		}

		static JSONObject ConvertJsonToObject(const std::string& json)
		{
			auto properties = ConvertJsonToProperties(json);
			return { properties };
		}

		JSONObject::JSONObject() : PROPERTIES({}) {}

		JSONObject::JSONObject(const std::vector<JSONProperty>& vals)
			: PROPERTIES(vals) {}

		JSONObject::JSONObject(const std::string& json) :
			PROPERTIES(ConvertJsonToProperties(json)) {}

		template<typename T>
		const std::optional<T> JSONObject::HasProperty(const std::string& propertyName) const
		{
			bool isList = IsListType<T>();
			for (const auto& property : PROPERTIES)
			{
				if (property.IS_LIST == isList && property.PROPERTY_NAME == propertyName)
				{
					return property.TryGetType<T>();
				}
			}
			return std::nullopt;
		}

		std::string JSONObject::ToString() const
		{
			std::cout << PROPERTIES.size();
			if (PROPERTIES.size()==0) return "[]";
			std::string result = "[";
			std::string propertyStr;

			int index = 0;
			for (const auto& property : PROPERTIES)
			{
				result += property.ToString();
				if (index >= PROPERTIES.size() - 1) continue;

				result += ", ";
				index++;
			}

			result += "]";
			return result;
		}

		template const std::optional<int> JSONObject::HasProperty(const std::string&) const;
		template const std::optional<double> JSONObject::HasProperty(const std::string&) const;
		template const std::optional<std::string> JSONObject::HasProperty(const std::string&) const;
		template const std::optional<JSONObject> JSONObject::HasProperty(const std::string&) const;
		template const std::optional<JSONList> JSONObject::HasProperty(const std::string&) const;
	}
}